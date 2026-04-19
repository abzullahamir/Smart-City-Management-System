#include "PublicFacilitiesSystem.h"

using namespace std;

// ============ helpers ============

string PublicFacilitiesSystem::toLower(const string& s) const {
    string res = s;
    for (size_t i = 0; i < res.length(); i++) {
        if (res[i] >= 'A' && res[i] <= 'Z') {
            res[i] = static_cast<char>(res[i] + 32);
        }
    }
    return res;
}

// "MOSQUE", "PARK", "WATERCOOLER", "ANY"
string PublicFacilitiesSystem::normalizeType(const string& type) const {
    string t = toLower(type);
    string compact;
    for (char c : t) {
        if (c != ' ' && c != '\t' && c != '_' && c != '-') {
            compact += c;
        }
    }

    if (compact.empty() || compact == "any") return "ANY";
    if (compact == "mosque" || compact == "mosques") return "MOSQUE";
    if (compact == "park" || compact == "parks") return "PARK";
    if (compact == "watercooler" || compact == "watercoolers") return "WATER_COOLER";

    
    string upper;
    for (char c : compact) {
        if (c >= 'a' && c <= 'z') upper += static_cast<char>(c - 32);
        else upper += c;
    }
    return upper;
}

double PublicFacilitiesSystem::calculateDistance(double lat1, double lon1,
    double lat2, double lon2) {
    double diffLat = (lat2 - lat1) * PI_FAC / 180.0;
    double diffLon = (lon2 - lon1) * PI_FAC / 180.0;

    lat1 = lat1 * PI_FAC / 180.0;
    lat2 = lat2 * PI_FAC / 180.0;

    double a = sin(diffLat / 2) * sin(diffLat / 2) +
        sin(diffLon / 2) * sin(diffLon / 2) * cos(lat1) * cos(lat2);

    if (a > 1.0) a = 1.0;
    if (a < 0.0) a = 0.0;

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS_FAC * c;
}

string PublicFacilitiesSystem::cleanString(string str) {
    if (str.empty()) return "";

    int first = 0;
    int last = static_cast<int>(str.length()) - 1;

    while (first < static_cast<int>(str.length()) &&
        (str[first] == ' ' || str[first] == '\t' ||
            str[first] == '\r' || str[first] == '\n' ||
            str[first] == '"')) {
        first++;
    }

    while (last >= first &&
        (str[last] == ' ' || str[last] == '\t' ||
            str[last] == '\r' || str[last] == '\n' ||
            str[last] == '"')) {
        last--;
    }

    if (first > last) return "";

    string result;
    for (int i = first; i <= last; i++) {
        result += str[i];
    }
    return result;
}

void PublicFacilitiesSystem::parseCoordinates(string coordStr,
    double& lat, double& lon) {
    lat = 0.0;
    lon = 0.0;

    coordStr = cleanString(coordStr);

    int commaPos = -1;
    for (int i = 0; i < static_cast<int>(coordStr.length()); i++) {
        if (coordStr[i] == ',') {
            commaPos = i;
            break;
        }
    }

    if (commaPos != -1) {
        string latStr = cleanString(coordStr.substr(0, commaPos));
        string lonStr = cleanString(coordStr.substr(commaPos + 1));

        try {
            lat = stod(latStr);
            lon = stod(lonStr);
        }
        catch (...) {
            cout << "Warning: Could not parse coordinates: "
                << coordStr << endl;
        }
    }
}

PublicFacility* PublicFacilitiesSystem::findFacilityByID(const string& id) {
    if (id.empty()) return nullptr;

    string target = toLower(id);
    PublicFacility* curr = facilityHead;
    while (curr != nullptr) {
        if (toLower(curr->facilityID) == target) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}


PublicFacilitiesSystem::PublicFacilitiesSystem(Graph* sharedGraph) {
    if (sharedGraph) {
        cityGraph = sharedGraph;
        ownsGraph = false;
    }
    else {
        cityGraph = new Graph();
        ownsGraph = true;
    }

    facilityHead = nullptr;
    facilityCount = 0;

    cout << "\n========================================\n";
    cout << "  PUBLIC FACILITIES SYSTEM INITIALIZED\n";
    cout << "========================================\n";
    cout << "Graph: " << (ownsGraph ? "Owned" : "Shared") << "\n";
    cout << "========================================\n\n";
}

PublicFacilitiesSystem::~PublicFacilitiesSystem() {
    cout << "\nCleaning up Public Facilities System...\n";

    PublicFacility* curr = facilityHead;
    while (curr != nullptr) {
        PublicFacility* temp = curr;
        curr = curr->next;
        delete temp;
    }

    if (ownsGraph) {
        delete cityGraph;
    }

    cout << "Public Facilities System cleaned up.\n";
}


Graph* PublicFacilitiesSystem::getCityGraph() {
    return cityGraph;
}

void PublicFacilitiesSystem::loadPublicFacilities(string filename) {
    cout << "\n--- Loading public facilities from: " << filename << " ---\n";

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << "\n";
        return;
    }

    string line;
    int count = 0;

    getline(file, line); // skip header

    while (getline(file, line)) {
        if (line.empty()) continue;

        string id, name, type, sector, coordinates;

        int fieldCount = 0;
        string currentField;
        bool insideQuotes = false;

        for (int i = 0; i < static_cast<int>(line.length()); i++) {
            char c = line[i];

            if (c == '"') {
                insideQuotes = !insideQuotes;
            }
            else if (c == ',' && !insideQuotes) {
                if (fieldCount == 0) id = cleanString(currentField);
                else if (fieldCount == 1) name = cleanString(currentField);
                else if (fieldCount == 2) type = cleanString(currentField);
                else if (fieldCount == 3) sector = cleanString(currentField);

                currentField.clear();
                fieldCount++;
            }
            else {
                currentField += c;
            }
        }

        coordinates = currentField;

        double lat = 0.0, lon = 0.0;
        parseCoordinates(coordinates, lat, lon);

        registerFacility(id, name, type, sector, lat, lon);
        count++;
    }

    file.close();
    cout << "\nLoaded " << count << " public facilities successfully.\n";
}

void PublicFacilitiesSystem::registerFacility(string id, string name,
    string type, string sector,
    double lat, double lon) {
    if (id.empty()) {
        cout << "Error: Facility ID cannot be empty.\n";
        return;
    }

    // prevent duplicates (case-insensitive)
    if (findFacilityByID(id) != nullptr) {
        cout << "Error: Facility ID '" << id << "' already exists.\n";
        return;
    }

    string normType = normalizeType(type);

    cout << "Registering facility: " << name << " (" << id << ")...\n";

    PublicFacility* facility =
        new PublicFacility(id, name, normType, sector, lat, lon);

    facility->next = facilityHead;
    facilityHead = facility;
    facilityCount++;

    // node type is the normalized facility type
    Node facilityNode(id, name, normType, lat, lon);
    cityGraph->addNode(facilityNode);

    // connect to nearby bus stops; node type is "BUSSTOP" in Graph
    cityGraph->connectToNearbyNodes(id, 5.0, "BUS_STOP");

    cout << "Facility " << name << " registered with bus connectivity!\n";
}

void PublicFacilitiesSystem::displayAllFacilities() {
    cout << "\n========== ALL PUBLIC FACILITIES ==========\n";

    if (facilityHead == nullptr) {
        cout << "No facilities registered.\n";
        return;
    }

    int mosques = 0, parks = 0, waterCoolers = 0, others = 0;

    PublicFacility* curr = facilityHead;
    while (curr != nullptr) {
        string t = normalizeType(curr->type);
        if (t == "MOSQUE") mosques++;
        else if (t == "PARK") parks++;
        else if (t == "WATERCOOLER") waterCoolers++;
        else others++;
        curr = curr->next;
    }

    cout << "Total Facilities: " << facilityCount << "\n";
    cout << "   Mosques: " << mosques << "\n";
    cout << "   Parks: " << parks << "\n";
    cout << "   Water Coolers: " << waterCoolers << "\n";
    if (others > 0) cout << "   Others: " << others << "\n";
    cout << "\n";

    curr = facilityHead;
    int count = 1;
    while (curr != nullptr) {
        cout << "[" << count++ << "] " << curr->facilityID << " - " << curr->name
            << " [" << curr->type << "] (" << curr->sector << ")\n";
        curr = curr->next;
    }

    cout << "============================================\n";
}

void PublicFacilitiesSystem::displayFacilitiesByType(string type) {
    string normType = normalizeType(type);

    cout << "\n========== " << normType << " FACILITIES ==========\n";

    PublicFacility* curr = facilityHead;
    int count = 0;

    while (curr != nullptr) {
        if (normalizeType(curr->type) == normType) {
            count++;
            cout << "[" << count << "] " << curr->name
                << " (" << curr->sector << ")\n";
            cout << "    Location: (" << curr->latitude
                << ", " << curr->longitude << ")\n\n";
        }
        curr = curr->next;
    }

    if (count == 0) {
        cout << "No " << normType << " facilities found.\n";
    }
    else {
        cout << "Total " << normType << ": " << count << "\n";
    }
    cout << "============================================\n";
}

void PublicFacilitiesSystem::viewFacility(string facilityID) {
    cout << "\n--- Viewing Facility: " << facilityID << " ---\n";

    PublicFacility* fac = findFacilityByID(facilityID);
    if (fac != nullptr) {
        fac->displayDetails();
    }
    else {
        cout << "Facility " << facilityID << " not found.\n";
    }
}

PublicFacility* PublicFacilitiesSystem::findNearestFacility(double lat,
    double lon,
    string typeFilter) {
    string normFilter = normalizeType(typeFilter);

    cout << "\n--- Finding Nearest ";
    if (normFilter != "ANY") cout << normFilter;
    else cout << "Facility";
    cout << " to (" << lat << ", " << lon << ") ---\n";

    PublicFacility* nearest = nullptr;
    double minDistance = -1.0;

    PublicFacility* curr = facilityHead;
    while (curr != nullptr) {
        string t = normalizeType(curr->type);
        if (normFilter == "ANY" || t == normFilter) {
            double dist =
                calculateDistance(lat, lon, curr->latitude, curr->longitude);
            if (minDistance < 0 || dist < minDistance) {
                minDistance = dist;
                nearest = curr;
            }
        }
        curr = curr->next;
    }

    if (nearest != nullptr) {
        cout << "Nearest: " << nearest->name << "\n";
        cout << "  Type: " << nearest->type << "\n";
        cout << "  Distance: " << minDistance << " km\n";
        cout << "  Sector: " << nearest->sector << "\n";
    }
    else {
        cout << "No facilities found.\n";
    }

    return nearest;
}

void PublicFacilitiesSystem::findNearestFacilityByName(string name) {
    cout << "\n--- Searching for Facility: " << name << " ---\n";

    PublicFacility* curr = facilityHead;
    while (curr != nullptr) {
        bool match = false;
        if (curr->name.length() >= name.length()) {
            for (size_t i = 0;
                i + name.length() <= curr->name.length();
                i++) {
                bool found = true;
                for (size_t j = 0; j < name.length(); j++) {
                    char c1 = curr->name[i + j];
                    char c2 = name[j];
                    if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
                    if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
                    if (c1 != c2) {
                        found = false;
                        break;
                    }
                }
                if (found) {
                    match = true;
                    break;
                }
            }
        }

        if (match) {
            cout << "\nFound: " << curr->name << "\n";
            cout << "  ID: " << curr->facilityID << "\n";
            cout << "  Type: " << curr->type << "\n";
            cout << "  Sector: " << curr->sector << "\n";
            cout << "  Location: (" << curr->latitude
                << ", " << curr->longitude << ")\n";
            return;
        }
        curr = curr->next;
    }

    cout << "Facility containing '" << name << "' not found.\n";
}

void PublicFacilitiesSystem::getTopNearestFacilities(double lat, double lon,
    int topN,
    string typeFilter) {
    string normFilter = normalizeType(typeFilter);

    cout << "\n--- Top " << topN << " Nearest ";
    if (normFilter != "ANY") cout << normFilter << " ";
    cout << "Facilities ---\n";
    cout << "Location: (" << lat << ", " << lon << ")\n\n";

    const int MAX_FAC = 100;
    PublicFacility* facList[MAX_FAC];
    double distances[MAX_FAC];
    int count = 0;

    PublicFacility* curr = facilityHead;
    while (curr != nullptr && count < MAX_FAC) {
        string t = normalizeType(curr->type);
        if (normFilter == "ANY" || t == normFilter) {
            facList[count] = curr;
            distances[count] =
                calculateDistance(lat, lon, curr->latitude, curr->longitude);
            count++;
        }
        curr = curr->next;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (distances[j] > distances[j + 1]) {
                double tempDist = distances[j];
                distances[j] = distances[j + 1];
                distances[j + 1] = tempDist;

                PublicFacility* tempFac = facList[j];
                facList[j] = facList[j + 1];
                facList[j + 1] = tempFac;
            }
        }
    }

    int limit = (topN < count) ? topN : count;

    for (int i = 0; i < limit; i++) {
        cout << (i + 1) << ". " << facList[i]->name
            << " [" << facList[i]->type << "]\n";
        cout << "   Distance: " << distances[i] << " km\n";
        cout << "   Sector: " << facList[i]->sector << "\n\n";
    }

    if (count == 0) {
        cout << "No facilities found.\n";
    }
}

void PublicFacilitiesSystem::findRouteToFacility(string fromNodeID,
    string facilityID) {
    cout << "\n--- Finding Route to Facility ---\n";

    PublicFacility* fac = findFacilityByID(facilityID);
    if (fac == nullptr) {
        cout << "Facility not found!\n";
        return;
    }

    cout << "Finding route to: " << fac->name
        << " (" << fac->sector << ")\n";

    // use canonical facility ID stored in the node, not raw user input
    cityGraph->dijkstraAlgo(fromNodeID, fac->facilityID, "ANY");
}

void PublicFacilitiesSystem::displayMenu() {
    cout << endl << "========================================" << endl;
    cout << "     PUBLIC FACILITIES SYSTEM" << endl;
    cout << "========================================" << endl;
    cout << "1.  Register New Facility" << endl;
    cout << "2.  View All Facilities" << endl;
    cout << "3.  View Facility by ID" << endl;
    cout << "4.  View Mosques" << endl;
    cout << "5.  View Parks" << endl;
    cout << "6.  View Water Coolers" << endl;
    cout << "7.  Find Nearest Facility (Any Type)" << endl;
    cout << "8.  Find Nearest Mosque" << endl;
    cout << "9.  Find Nearest Park" << endl;
    cout << "10. Find Nearest Water Cooler" << endl;
    cout << "11. Search Facility by Name" << endl;
    cout << "12. Top N Nearest Facilities" << endl;
    cout << "13. Find Route to Facility" << endl;
    cout << "0.  Back to Main Menu" << endl;
    cout << "========================================" << endl;
    cout << "Enter choice: ";
}

void PublicFacilitiesSystem::run() {
    int choice;

    while (true) {
        displayMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore();

        if (choice == 0) {
            cout << "Returning to main menu..." << endl;
            break;
        }

        switch (choice) {
        case 1: {
            string id, name, type, sector;
            double lat, lon;

            cout << "Enter Facility ID: ";
            getline(cin, id);

            cout << "Enter Name: ";
            getline(cin, name);

            int typeChoice;
            cout << "Select Type:\n";
            cout << "  1. Mosque\n";
            cout << "  2. Park\n";
            cout << "  3. Water Cooler\n";
            cout << "Enter choice (1-3): ";
            if (!(cin >> typeChoice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Registration cancelled.\n";
                break;
            }
            cin.ignore();

            if (typeChoice == 1)      type = "MOSQUE";
            else if (typeChoice == 2) type = "PARK";
            else if (typeChoice == 3) type = "WATERCOOLER";
            else {
                cout << "Invalid type choice. Registration cancelled.\n";
                break;
            }

            cout << "Enter Sector: ";
            getline(cin, sector);

            cout << "Enter Latitude: ";
            cin >> lat;
            cout << "Enter Longitude: ";
            cin >> lon;
            cin.ignore();

            registerFacility(id, name, type, sector, lat, lon);
            break;
        }

        case 2:
            displayAllFacilities();
            break;
        case 3: {
            string id;
            cout << "Enter Facility ID: ";
            getline(cin, id);
            viewFacility(id);
            break;
        }
        case 4:
            displayFacilitiesByType("MOSQUE");
            break;
        case 5:
            displayFacilitiesByType("PARK");
            break;
        case 6:
            displayFacilitiesByType("WATER_COOLER");
            break;
        case 7: {
            double lat, lon;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cin.ignore();
            findNearestFacility(lat, lon, "ANY");
            break;
        }
        case 8: {
            double lat, lon;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cin.ignore();
            findNearestFacility(lat, lon, "MOSQUE");
            break;
        }
        case 9: {
            double lat, lon;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cin.ignore();
            findNearestFacility(lat, lon, "PARK");
            break;
        }
        case 10: {
            double lat, lon;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cin.ignore();
            findNearestFacility(lat, lon, "WATER_COOLER");
            break;
        }
        case 11: {
            string name;
            cout << "Enter facility name to search: ";
            getline(cin, name);
            findNearestFacilityByName(name);
            break;
        }
        case 12: {
            double lat, lon;
            int n;
            string type;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cout << "Enter N (how many): ";
            cin >> n;
            cin.ignore();
            cout << "Enter type filter (MOSQUE/PARK/WATER_COOLER/ANY): ";
            getline(cin, type);
            getTopNearestFacilities(lat, lon, n, type);
            break;
        }
        case 13: {
            string fromID, facID;
            cout << "Enter your current Node ID (e.g., bus stop): ";
            getline(cin, fromID);
            cout << "Enter Facility ID: ";
            getline(cin, facID);
            findRouteToFacility(fromID, facID);
            break;
        }
        default:
            cout << "Invalid choice! Try again." << endl;
        }
    }
}
