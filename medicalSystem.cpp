#include "medicalSystem.h"

using namespace std;

// ============ HELPER FUNCTIONS ============

double MedicalSystem::calculateDistance(double lat1, double lon1,
    double lat2, double lon2) {
    double diffLat = (lat2 - lat1) * PI_MED / 180.0;
    double diffLon = (lon2 - lon1) * PI_MED / 180.0;

    lat1 = lat1 * PI_MED / 180.0;
    lat2 = lat2 * PI_MED / 180.0;

    double a = sin(diffLat / 2) * sin(diffLat / 2) +
        sin(diffLon / 2) * sin(diffLon / 2) * cos(lat1) * cos(lat2);

    if (a > 1.0) a = 1.0;
    if (a < 0.0) a = 0.0;

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS_MED * c;
}

string MedicalSystem::cleanString(string str) {
    if (str.empty()) return "";

    int first = 0;
    int last = static_cast<int>(str.length()) - 1;

    while (first < static_cast<int>(str.length()) &&
        (str[first] == ' ' || str[first] == '\t' ||
            str[first] == '\r' || str[first] == '\n')) {
        first++;
    }

    while (last >= first &&
        (str[last] == ' ' || str[last] == '\t' ||
            str[last] == '\r' || str[last] == '\n')) {
        last--;
    }

    if (first > last) return "";

    string result = "";
    for (int i = first; i <= last; i++) {
        result += str[i];
    }
    return result;
}

void MedicalSystem::parseCoordinates(string coordStr, double& lat, double& lon) {
    lat = 0.0;
    lon = 0.0;

    if (!coordStr.empty() && coordStr[0] == '\"' && coordStr.length() >= 2) {
        coordStr = coordStr.substr(1, coordStr.length() - 2);
    }

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
            cout << "Warning: Could not parse coordinates: " << coordStr << endl;
            lat = 0.0;
            lon = 0.0;
        }
    }
}

int MedicalSystem::stringToInt(string str) {
    str = cleanString(str);
    if (str.empty()) return 0;

    int result = 0;
    bool negative = false;
    int i = 0;

    if (str[0] == '-') {
        negative = true;
        i = 1;
    }

    for (; i < static_cast<int>(str.length()); i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        }
        else {
            break;
        }
    }

    return negative ? -result : result;
}

PharmacyNode* MedicalSystem::findPharmacyNode(const string& pharmacyID) {
    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->pharm->getID(), pharmacyID)) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}

string MedicalSystem::toLower(const string& s) {
    string res = s;
    for (size_t i = 0; i < res.length(); i++) {
        if (res[i] >= 'A' && res[i] <= 'Z') {
            res[i] = static_cast<char>(res[i] + 32);
        }
    }
    return res;
}

bool MedicalSystem::equalsIgnoreCase(const string& a, const string& b) {
    return toLower(a) == toLower(b);
}

bool MedicalSystem::containsIgnoreCase(const string& text, const string& pattern) {
    string t = toLower(text);
    string p = toLower(pattern);

    if (p.empty()) return true;
    if (p.length() > t.length()) return false;

    for (size_t i = 0; i + p.length() <= t.length(); i++) {
        bool match = true;
        for (size_t j = 0; j < p.length(); j++) {
            if (t[i + j] != p[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

// ============ CONSTRUCTOR / DESTRUCTOR ============

MedicalSystem::MedicalSystem(Graph* sharedGraph,
    int hospitalHashSize,
    int doctorHashSize,
    int heapCapacity) {
    if (sharedGraph) {
        cityGraph = sharedGraph;
        ownsGraph = false;
    }
    else {
        cityGraph = new Graph();
        ownsGraph = true;
    }

    hospitalHash = new HospitalHashTable(hospitalHashSize);
    doctorHash = new DoctorHashTable(doctorHashSize);
    bedsHeap = new HospitalMaxHeap(heapCapacity);
    pharmacyHead = nullptr;
    pharmacyCount = 0;

    cout << "\n========================================\n";
    cout << " MEDICAL SYSTEM INITIALIZED\n";
    cout << "========================================\n";
    cout << "Hospital Hash Size: " << hospitalHashSize << "\n";
    cout << "Doctor Hash Size: " << doctorHashSize << "\n";
    cout << "Emergency Heap Capacity: " << heapCapacity << "\n";
    cout << "Graph: " << (ownsGraph ? "Owned" : "Shared") << "\n";
    cout << "========================================\n\n";
}

MedicalSystem::~MedicalSystem() {
    cout << "\nCleaning up Medical System...\n";

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        PharmacyNode* temp = curr;
        curr = curr->next;
        delete temp->pharm;
        delete temp;
    }

    delete hospitalHash;
    delete doctorHash;
    delete bedsHeap;

    if (ownsGraph) {
        delete cityGraph;
    }

    cout << "Medical System cleaned up.\n";
}

Graph* MedicalSystem::getCityGraph() {
    return cityGraph;
}

// ============ DATA LOADING ============

void MedicalSystem::loadDataFromFiles(string hospitalsFile, string pharmaciesFile) {
    cout << endl << "===== Loading Medical Data =====" << endl;
    loadHospitals(hospitalsFile);
    loadPharmacies(pharmaciesFile);
    cout << "Medical system data loaded successfully!" << endl;
}

void MedicalSystem::loadHospitals(string filename) {
    cout << "\n--- Loading hospitals from: " << filename << " ---\n";

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << "\n";
        return;
    }

    string line;
    int count = 0;

    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        string id, name, sector, bedsStr, spec, coordinates;
        int fieldCount = 0;
        string currentField = "";
        bool insideQuotes = false;

        for (int i = 0; i < static_cast<int>(line.length()); i++) {
            char c = line[i];
            if (c == '\"') {
                insideQuotes = !insideQuotes;
            }
            else if (c == ',' && !insideQuotes) {
                if (fieldCount == 0) id = cleanString(currentField);
                else if (fieldCount == 1) name = cleanString(currentField);
                else if (fieldCount == 2) sector = cleanString(currentField);
                else if (fieldCount == 3) bedsStr = cleanString(currentField);
                else if (fieldCount == 4) spec = cleanString(currentField);

                currentField = "";
                fieldCount++;
            }
            else {
                currentField += c;
            }
        }

        coordinates = currentField;

        double lat = 0.0, lon = 0.0;
        parseCoordinates(coordinates, lat, lon);

        int beds = stringToInt(bedsStr);

        registerHospital(id, name, sector, beds, spec, lat, lon);
        count++;
    }

    file.close();
    cout << "\nLoaded " << count << " hospitals successfully.\n";
}

void MedicalSystem::loadPharmacies(string filename) {
    cout << "\n--- Loading pharmacies from: " << filename << " ---\n";

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << "\n";
        return;
    }

    string line;
    int count = 0;

    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        string id, name, sector, medName, formula, priceStr, coordinates;
        int fieldCount = 0;
        string currentField = "";
        bool insideQuotes = false;

        for (int i = 0; i < static_cast<int>(line.length()); i++) {
            char c = line[i];
            if (c == '\"') {
                insideQuotes = !insideQuotes;
            }
            else if (c == ',' && !insideQuotes) {
                if (fieldCount == 0) id = cleanString(currentField);
                else if (fieldCount == 1) name = cleanString(currentField);
                else if (fieldCount == 2) sector = cleanString(currentField);
                else if (fieldCount == 3) medName = cleanString(currentField);
                else if (fieldCount == 4) formula = cleanString(currentField);
                else if (fieldCount == 5) priceStr = cleanString(currentField);

                currentField = "";
                fieldCount++;
            }
            else {
                currentField += c;
            }
        }

        coordinates = currentField;

        double lat = 0.0, lon = 0.0;
        parseCoordinates(coordinates, lat, lon);

        int price = stringToInt(priceStr);

        Medicine med(medName, formula, price);

        PharmacyNode* existing = findPharmacyNode(id);
        if (existing) {
            existing->pharm->addMedicine(med);
        }
        else {
            registerPharmacy(id, name, sector, lat, lon, med);
            count++;
        }
    }

    file.close();
    cout << "\nLoaded " << count << " pharmacies successfully.\n";
}

// ============ HOSPITAL MANAGEMENT ============

void MedicalSystem::registerHospital(string id, string name, string sector,
    int beds, string spec, double lat, double lon) {
    cout << "Registering hospital: " << name << " (" << id << ")...\n";

    Hospital* hospital = new Hospital(id, name, sector, beds, spec, lat, lon);

    if (!hospitalHash->insert(id, hospital)) {
        cout << "ERROR: Could not register hospital, ID already exists.\n";
        delete hospital;
        return;
    }

    bedsHeap->insert(hospital);

    Node hospitalNode(id, name, "HOSPITAL", lat, lon);
    cityGraph->addNode(hospitalNode);

    cityGraph->connectToNearbyNodes(id, 5.0, "BUS_STOP");

    cout << "Hospital " << name << " registered successfully!\n";
}

void MedicalSystem::deactivateHospital(string hospitalID) {
    cout << "\n--- Deactivating hospital: " << hospitalID << " ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    string hospitalName = h->name;

    bool removed = hospitalHash->remove(hospitalID);
    if (removed) {
        cout << "Removed from hash table lookups.\n";
    }

    h->emergencyBeds = -1;
    h->name = "[DEACTIVATED] " + h->name;
    cout << "Marked as deactivated.\n";

    bedsHeap->rebuildHeap();
    cout << "Emergency beds heap rebuilt.\n";

    cout << "Hospital " << hospitalName << " deactivated successfully.\n";
}

void MedicalSystem::reactivateHospital(string hospitalID) {
    cout << "\n--- Reactivating hospital: " << hospitalID << " ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found. May need to re-register.\n";
        return;
    }

    if (h->emergencyBeds != -1) {
        cout << "Hospital is already active.\n";
        return;
    }

    string originalName = h->name;
    if (originalName.find("[DEACTIVATED] ") == 0) {
        string newName = "";
        for (size_t i = 14; i < originalName.length(); i++) {
            newName += originalName[i];
        }
        h->name = newName;
    }

    cout << "Enter emergency bed count for " << h->name << ": ";
    int newBeds;
    cin >> newBeds;
    if (newBeds < 0) {
        cout << "Invalid bed count. Reactivation cancelled.\n";
        return;
    }

    h->emergencyBeds = newBeds;

    hospitalHash->insert(hospitalID, h);
    cout << "Re-added to hash table lookups.\n";

    bedsHeap->rebuildHeap();
    cout << "Emergency beds heap rebuilt.\n";

    cout << "Hospital " << h->name << " reactivated with " << newBeds << " beds.\n";
}

void MedicalSystem::viewHospital(string hospitalID) {
    cout << "\n--- Viewing Hospital: " << hospitalID << " ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h != nullptr) {
        h->displayHospitalDetails();
    }
    else {
        cout << "Hospital " << hospitalID << " not found.\n";
    }
}

void MedicalSystem::displayAllHospitals() {
    cout << "\n========== ALL HOSPITALS ==========\n";
    hospitalHash->displayClean();
}

void MedicalSystem::displayNetwork() {
    cout << "\n========== Medical Sector Graph ==========\n";
    cityGraph->displayGraph("HOSPITAL");
    cout << "\n========== Medical Sector Pharmacies ==========\n";
    cityGraph->displayGraph("PHARMACY");
}

// ============ DOCTOR MANAGEMENT ============

void MedicalSystem::hireDoctor(string hospitalID, string doctorName, string specialization) {
    cout << "\n--- Hiring Doctor ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    Doctor* newDoc = new Doctor(doctorName, specialization);
    newDoc->next = h->doctorHead;
    h->doctorHead = newDoc;
    h->doctorCount++;

    doctorHash->insert(specialization, newDoc, h);

    cout << "Hired Dr. " << doctorName << " (" << specialization
        << ") at " << h->name << "\n";
}

void MedicalSystem::fireDoctor(string hospitalID, string doctorName) {
    cout << "\n--- Firing Doctor ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    Doctor* prev = nullptr;
    Doctor* curr = h->doctorHead;

    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->name, doctorName)) {
            if (prev == nullptr) {
                h->doctorHead = curr->next;
            }
            else {
                prev->next = curr->next;
            }

            h->doctorCount--;

            doctorHash->removeDoctor(curr->name, h->hospitalID);

            cout << "Fired Dr. " << curr->name << " from " << h->name << "\n";

            delete curr;
            return;
        }

        prev = curr;
        curr = curr->next;
    }

    cout << "Dr. " << doctorName << " not found at " << h->name << "\n";
}

void MedicalSystem::viewAllDoctors() {
    cout << "\n--- All Doctors in System ---\n";
    doctorHash->displayClean();
}

void MedicalSystem::viewDoctorsBySpecialization(string specialization) {
    doctorHash->searchBySpecialization(specialization);
}

void MedicalSystem::viewDoctorsAtHospital(string hospitalID) {
    cout << "\n--- Doctors at Hospital: " << hospitalID << " ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    cout << "Hospital: " << h->name << "\n";
    cout << "Total Doctors: " << h->doctorCount << "\n\n";

    if (h->doctorHead == nullptr) {
        cout << "No doctors currently employed.\n";
        return;
    }

    Doctor* d = h->doctorHead;
    int count = 1;
    while (d != nullptr) {
        cout << count++ << ". Dr. " << d->name
            << " - " << d->specialization << "\n";
        d = d->next;
    }
}

int MedicalSystem::countDoctorsBySpecialization(string specialization) {
    cout << "\n--- Counting doctors: " << specialization << " ---\n";

    const int MAX_RESULTS = 100;
    Doctor* doctors[MAX_RESULTS];
    Hospital* hospitals[MAX_RESULTS];

    int count = doctorHash->getDoctorsBySpecialization(
        specialization, doctors, hospitals, MAX_RESULTS
    );

    cout << "Total: " << count << " doctor(s) found.\n";
    return count;
}

void MedicalSystem::findHospitalsWithSpecialization(string specialization) {
    cout << "\n--- Hospitals with " << specialization << " specialists ---\n";

    const int MAX_RESULTS = 100;
    Doctor* doctors[MAX_RESULTS];
    Hospital* hospitals[MAX_RESULTS];

    int count = doctorHash->getDoctorsBySpecialization(
        specialization, doctors, hospitals, MAX_RESULTS
    );

    if (count == 0) {
        cout << "No " << specialization << " specialists found.\n";
        return;
    }

    cout << "Found at:\n";
    for (int i = 0; i < count; i++) {
        cout << " " << (i + 1) << ". " << hospitals[i]->name
            << " (" << hospitals[i]->sector << ") - Dr. "
            << doctors[i]->name << "\n";
    }
}

// ============ PATIENT MANAGEMENT ============

void MedicalSystem::admitPatient(string hospitalID, string patientName,
    int age, string condition) {
    cout << "\n--- Admitting Patient ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    if (h->emergencyBeds <= 0) {
        cout << "Warning: No emergency beds available at " << h->name << "\n";
    }

    Patient* newPat = new Patient(patientName, age, condition);
    newPat->next = h->patientHead;
    h->patientHead = newPat;
    h->patientCount++;

    cout << "Admitted " << patientName << " (Age: " << age
        << ", Condition: " << condition << ") to " << h->name << "\n";
}

void MedicalSystem::dischargePatient(string hospitalID, string patientName) {
    cout << "\n--- Discharging Patient ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    Patient* prev = nullptr;
    Patient* curr = h->patientHead;

    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->name, patientName)) {
            if (prev == nullptr) {
                h->patientHead = curr->next;
            }
            else {
                prev->next = curr->next;
            }

            h->patientCount--;

            cout << "Discharged " << patientName << " from " << h->name << "\n";

            delete curr;
            return;
        }

        prev = curr;
        curr = curr->next;
    }

    cout << "Patient " << patientName << " not found at " << h->name << "\n";
}

void MedicalSystem::viewPatientsAtHospital(string hospitalID) {
    cout << "\n--- Patients at Hospital: " << hospitalID << " ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    cout << "Hospital: " << h->name << "\n";
    cout << "Total Patients: " << h->patientCount << "\n\n";

    if (h->patientHead == nullptr) {
        cout << "No patients currently admitted.\n";
        return;
    }

    Patient* p = h->patientHead;
    int count = 1;
    while (p != nullptr) {
        cout << count++ << ". " << p->name
            << " (Age: " << p->age
            << ", Condition: " << p->condition << ")\n";
        p = p->next;
    }
}

void MedicalSystem::viewAllPatients() {
    cout << "\n========== ALL PATIENTS IN SYSTEM ==========\n";

    int totalPatients = 0;
    int hospitalsWithPatients = 0;

    const int MAX_HOSPITALS = 100;
    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds != -1 && h->patientCount > 0) {
            cout << "\n[" << h->name << " - " << h->sector << "]\n";

            Patient* p = h->patientHead;
            int count = 1;
            while (p != nullptr) {
                cout << " " << count++ << ". " << p->name
                    << " (Age: " << p->age
                    << ", Condition: " << p->condition << ")\n";
                p = p->next;
                totalPatients++;
            }

            hospitalsWithPatients++;
        }
    }

    cout << "\n====================================\n";
    cout << "Total Patients: " << totalPatients << "\n";
    cout << "Hospitals with Patients: " << hospitalsWithPatients << "\n";
}

void MedicalSystem::transferPatient(string fromHospitalID, string toHospitalID,
    string patientName) {
    cout << "\n--- Transferring Patient ---\n";

    Hospital* fromH = hospitalHash->search(fromHospitalID);
    Hospital* toH = hospitalHash->search(toHospitalID);

    if (fromH == nullptr) {
        cout << "Source hospital " << fromHospitalID << " not found.\n";
        return;
    }
    if (toH == nullptr) {
        cout << "Destination hospital " << toHospitalID << " not found.\n";
        return;
    }

    Patient* prev = nullptr;
    Patient* curr = fromH->patientHead;

    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->name, patientName)) {
            if (prev == nullptr) {
                fromH->patientHead = curr->next;
            }
            else {
                prev->next = curr->next;
            }

            fromH->patientCount--;

            curr->next = toH->patientHead;
            toH->patientHead = curr;
            toH->patientCount++;

            cout << "Transferred " << patientName << " from "
                << fromH->name << " to " << toH->name << "\n";
            return;
        }

        prev = curr;
        curr = curr->next;
    }

    cout << "Patient " << patientName << " not found at " << fromH->name << "\n";
}

void MedicalSystem::findPatient(string patientName) {
    cout << "\n--- Searching for Patient: " << patientName << " ---\n";

    bool found = false;

    const int MAX_HOSPITALS = 100;
    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds != -1) {
            Patient* p = h->patientHead;
            while (p != nullptr) {
                if (equalsIgnoreCase(p->name, patientName)) {
                    cout << "Found: " << patientName << "\n";
                    cout << " Hospital: " << h->name
                        << " (" << h->hospitalID << ")\n";
                    cout << " Sector: " << h->sector << "\n";
                    cout << " Age: " << p->age << "\n";
                    cout << " Condition: " << p->condition << "\n";
                    found = true;
                }
                p = p->next;
            }
        }
    }

    if (!found) {
        cout << "Patient " << patientName << " not found in any hospital.\n";
    }
}

void MedicalSystem::getPatientStatistics(string hospitalID) {
    cout << "\n--- Patient Statistics for Hospital: " << hospitalID << " ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    cout << "Hospital: " << h->name << "\n";
    cout << "Total Patients: " << h->patientCount << "\n";

    if (h->patientCount == 0) {
        cout << "No patients to analyze.\n";
        return;
    }

    int totalAge = 0;
    int minAge = 999;
    int maxAge = 0;

    Patient* p = h->patientHead;
    while (p != nullptr) {
        totalAge += p->age;
        if (p->age < minAge) minAge = p->age;
        if (p->age > maxAge) maxAge = p->age;
        p = p->next;
    }

    double avgAge = static_cast<double>(totalAge) / h->patientCount;
    cout << "Average Age: " << avgAge << "\n";
    cout << "Youngest: " << minAge << " years\n";
    cout << "Oldest: " << maxAge << " years\n";
}

// ============ EMERGENCY BED MANAGEMENT ============

void MedicalSystem::viewMaxEmergencyBeds() {
    cout << "\n--- Hospital with Most Emergency Beds ---\n";

    Hospital* top = bedsHeap->peek();
    if (top == nullptr || top->emergencyBeds <= 0) {
        cout << "No hospitals have available emergency beds.\n";
        return;
    }

    cout << "Hospital: " << top->name << "\n";
    cout << "ID: " << top->hospitalID << "\n";
    cout << "Sector: " << top->sector << "\n";
    cout << "Emergency Beds: " << top->emergencyBeds << "\n";
    cout << "Specialization: " << top->specialization << "\n";
}

void MedicalSystem::viewAllEmergencyBeds() {
    cout << "\n========== EMERGENCY BEDS AVAILABILITY ==========\n";
   //unsorted, print as max heap (parent >children)
   //bedsHeap->display();
    
	// sorted display
    bedsHeap->displaySortedByBeds();
    cout << "=================================================\n";
}

void MedicalSystem::changeEmergencyBedCount(string hospitalID, int newBedCount) {
    cout << "\n--- Changing Emergency Bed Count ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    if (newBedCount < 0) {
        cout << "Invalid bed count. Must be >= 0.\n";
        return;
    }

    int oldCount = h->emergencyBeds;
    h->emergencyBeds = newBedCount;

    bedsHeap->rebuildHeap();

    cout << "Updated " << h->name << " emergency beds: "
        << oldCount << " -> " << newBedCount << "\n";
}

void MedicalSystem::reserveEmergencyBed(string hospitalID) {
    cout << "\n--- Reserving Emergency Bed ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    cout << "Enter number of beds to reserve: ";
    int amount;
    cin >> amount;
    cin.ignore();

    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }

    if (h->emergencyBeds < amount) {
        cout << "No emergency beds available at " << h->name << " for this amount.\n";
        return;
    }

    changeEmergencyBedCount(hospitalID, h->emergencyBeds - amount);
    cout << "Reserved " << amount << " emergency bed(s). Remaining: "
        << h->emergencyBeds << "\n";
}

void MedicalSystem::releaseEmergencyBed(string hospitalID) {
    cout << "\n--- Releasing Emergency Bed ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital " << hospitalID << " not found.\n";
        return;
    }

    cout << "Enter number of beds to release: ";
    int amount;
    cin >> amount;
    cin.ignore();

    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }

    changeEmergencyBedCount(hospitalID, h->emergencyBeds + amount);
    cout << "Released " << amount << " emergency bed(s). Available: "
        << h->emergencyBeds << "\n";
}

void MedicalSystem::getEmergencyBedStatistics() {
    cout << "\n--- Emergency Bed Statistics ---\n";

    int totalBeds = 0;
    int hospitalsWithBeds = 0;
    int hospitalsWithNoBeds = 0;
    int maxBeds = 0;
    int minBeds = 999;
    string maxHospital = "";
    string minHospital = "";
    int totalHospitals = 0;

    const int MAX_HOSPITALS = 100;
    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds != -1) {
            totalHospitals++;
            totalBeds += h->emergencyBeds;

            if (h->emergencyBeds > 0) hospitalsWithBeds++;
            else hospitalsWithNoBeds++;

            if (h->emergencyBeds > maxBeds) {
                maxBeds = h->emergencyBeds;
                maxHospital = h->name;
            }

            if (h->emergencyBeds < minBeds) {
                minBeds = h->emergencyBeds;
                minHospital = h->name;
            }
        }
    }

    if (totalHospitals == 0) {
        cout << "No hospitals in system.\n";
        return;
    }

    double avgBeds = static_cast<double>(totalBeds) / totalHospitals;

    cout << "Total Hospitals: " << totalHospitals << "\n";
    cout << "Total Emergency Beds: " << totalBeds << "\n";
    cout << "Average Beds/Hospital: " << avgBeds << "\n";
    cout << "With Available Beds: " << hospitalsWithBeds << "\n";
    cout << "With No Beds: " << hospitalsWithNoBeds << "\n";
    cout << "Most Beds: " << maxBeds << " (" << maxHospital << ")\n";
    cout << "Least Beds: " << minBeds << " (" << minHospital << ")\n";
}

void MedicalSystem::findHospitalsWithMinimumBeds(int minBeds) {
    cout << "\n--- Hospitals with >= " << minBeds << " Emergency Beds ---\n";

    bool found = false;
    int count = 0;

    const int MAX_HOSPITALS = 100;
    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds >= minBeds) {
            count++;
            cout << count << ". " << h->name << " (" << h->hospitalID << ")\n";
            cout << " Sector: " << h->sector << "\n";
            cout << " Available Beds: " << h->emergencyBeds << "\n\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No hospitals found with " << minBeds << "+ emergency beds.\n";
    }
}

void MedicalSystem::alertLowBedAvailability(int threshold) {
    cout << "\n========== LOW BED AVAILABILITY ALERT ==========\n";
    cout << "Threshold: " << threshold << " beds\n\n";

    bool alertFound = false;

    const int MAX_HOSPITALS = 100;
    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds != -1 &&
            h->emergencyBeds < threshold) {
            cout << "WARNING: " << h->name << " (" << h->sector << ")\n";
            cout << " Only " << h->emergencyBeds << " bed(s) available\n";
            alertFound = true;
        }
    }

    if (!alertFound) {
        cout << "All hospitals have adequate bed availability.\n";
    }

    cout << "================================================\n";
}

// ============ NEAREST HOSPITAL & ROUTING ============

Hospital* MedicalSystem::findNearestHospital(double lat, double lon) {
    cout << "\n--- Finding Nearest Hospital to (" << lat << ", " << lon << ") ---\n";

    string nearestID = cityGraph->findNearestNode(lat, lon, "HOSPITAL");
    if (nearestID.empty()) {
        cout << "No hospitals found in system.\n";
        return nullptr;
    }

    Hospital* nearest = hospitalHash->search(nearestID);
    if (nearest != nullptr) {
        double dist = calculateDistance(lat, lon,
            nearest->latitude, nearest->longitude);
        cout << "Nearest Hospital: " << nearest->name << "\n";
        cout << " Distance: " << dist << " km\n";
        cout << " Sector: " << nearest->sector << "\n";
        cout << " Emergency Beds: " << nearest->emergencyBeds << "\n";
    }
    else {
        cout << "Hospital metadata for " << nearestID << " not found.\n";
    }

    return nearest;
}

Hospital* MedicalSystem::findNearestHospitalWithBeds(double lat, double lon, int minBeds) {
    cout << "\n--- Finding Nearest Hospital with >= " << minBeds << " Beds ---\n";
    cout << "Location: (" << lat << ", " << lon << ")\n";

    Hospital* nearest = nullptr;
    double minDistance = -1.0;

    const int MAX_HOSPITALS = 100;
    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds >= minBeds) {
            double dist = calculateDistance(lat, lon, h->latitude, h->longitude);
            if (minDistance < 0 || dist < minDistance) {
                minDistance = dist;
                nearest = h;
            }
        }
    }

    if (nearest != nullptr) {
        cout << "Found: " << nearest->name << "\n";
        cout << " Distance: " << minDistance << " km\n";
        cout << " Available Beds: " << nearest->emergencyBeds << "\n";
        cout << " Sector: " << nearest->sector << "\n";
    }
    else {
        cout << "No hospital with " << minBeds << "+ beds found.\n";
    }

    return nearest;
}

Hospital* MedicalSystem::findNearestHospitalWithSpecialization(double lat, double lon,
    string specialization) {
    cout << "\n--- Finding Nearest Hospital with: " << specialization << " ---\n";
    cout << "Location: (" << lat << ", " << lon << ")\n";

    Hospital* nearest = nullptr;
    double minDistance = -1.0;

    const int MAX_HOSPITALS = 100;
    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds != -1) {
            if (containsIgnoreCase(h->specialization, specialization)) {
                double dist = calculateDistance(lat, lon,
                    h->latitude, h->longitude);
                if (minDistance < 0 || dist < minDistance) {
                    minDistance = dist;
                    nearest = h;
                }
            }
        }
    }

    if (nearest != nullptr) {
        cout << "Found: " << nearest->name << "\n";
        cout << " Distance: " << minDistance << " km\n";
        cout << " Specialization: " << nearest->specialization << "\n";
        cout << " Emergency Beds: " << nearest->emergencyBeds << "\n";
    }
    else {
        cout << "No hospital with " << specialization << " found.\n";
    }

    return nearest;
}

Hospital* MedicalSystem::findNearestHospitalWithDoctorSpecialization(double lat, double lon,
    string doctorSpec) {
    cout << "\n--- Finding Nearest Hospital with " << doctorSpec << " Doctor ---\n";
    cout << "Location: (" << lat << ", " << lon << ")\n";

    const int MAX_RESULTS = 100;
    Doctor* doctors[MAX_RESULTS];
    Hospital* hospitals[MAX_RESULTS];

    int count = doctorHash->getDoctorsBySpecialization(
        doctorSpec, doctors, hospitals, MAX_RESULTS
    );

    if (count == 0) {
        cout << "No " << doctorSpec << " doctors found in system.\n";
        return nullptr;
    }

    Hospital* nearest = nullptr;
    double minDistance = -1.0;

    for (int i = 0; i < count; i++) {
        Hospital* h = hospitals[i];
        if (h != nullptr && h->emergencyBeds != -1) {
            double dist = calculateDistance(lat, lon,
                h->latitude, h->longitude);
            if (minDistance < 0 || dist < minDistance) {
                minDistance = dist;
                nearest = h;
            }
        }
    }

    if (nearest != nullptr) {
        cout << "Found: " << nearest->name << "\n";
        cout << " Distance: " << minDistance << " km\n";
        cout << " " << doctorSpec << " doctors available\n";
        cout << " Emergency Beds: " << nearest->emergencyBeds << "\n";
    }

    return nearest;
}

Hospital* MedicalSystem::findOptimalHospital(double lat, double lon,
    string specialization, int minBeds) {
    cout << "\n--- Finding Optimal Hospital ---\n";
    cout << "Location: (" << lat << ", " << lon << ")\n";
    cout << "Requirements: " << specialization << ", " << minBeds << "+ beds\n";

    Hospital* optimal = nullptr;
    double minDistance = -1.0;

    const int MAX_HOSPITALS = 100;
    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds >= minBeds) {
            if (containsIgnoreCase(h->specialization, specialization)) {
                double dist = calculateDistance(lat, lon,
                    h->latitude, h->longitude);
                if (minDistance < 0 || dist < minDistance) {
                    minDistance = dist;
                    optimal = h;
                }
            }
        }
    }

    if (optimal != nullptr) {
        cout << "\nOptimal Hospital: " << optimal->name << "\n";
        cout << " Distance: " << minDistance << " km\n";
        cout << " Specialization: " << optimal->specialization << "\n";
        cout << " Available Beds: " << optimal->emergencyBeds << "\n";
    }
    else {
        cout << "No hospital meets all requirements.\n";
    }

    return optimal;
}

void MedicalSystem::getTopNearestHospitals(double lat, double lon, int topN) {
    cout << "\n--- Top " << topN << " Nearest Hospitals ---\n";
    cout << "Location: (" << lat << ", " << lon << ")\n\n";

    const int MAX_HOSPITALS = 100;
    Hospital* hospitalList[MAX_HOSPITALS];
    double distances[MAX_HOSPITALS];
    int count = 0;

    Hospital* allHospitals[MAX_HOSPITALS];
    int hospitalCount = hospitalHash->getAllHospitals(allHospitals, MAX_HOSPITALS);

    for (int i = 0; i < hospitalCount && count < MAX_HOSPITALS; i++) {
        Hospital* h = allHospitals[i];
        if (h != nullptr && h->emergencyBeds != -1) {
            hospitalList[count] = h;
            distances[count] = calculateDistance(
                lat, lon, h->latitude, h->longitude
            );
            count++;
        }
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (distances[j] > distances[j + 1]) {
                double tempDist = distances[j];
                distances[j] = distances[j + 1];
                distances[j + 1] = tempDist;

                Hospital* tempHosp = hospitalList[j];
                hospitalList[j] = hospitalList[j + 1];
                hospitalList[j + 1] = tempHosp;
            }
        }
    }

    int limit = (topN < count) ? topN : count;

    for (int i = 0; i < limit; i++) {
        cout << (i + 1) << ". " << hospitalList[i]->name << "\n";
        cout << " Distance: " << distances[i] << " km\n";
        cout << " Sector: " << hospitalList[i]->sector << "\n";
        cout << " Beds: " << hospitalList[i]->emergencyBeds << "\n\n";
    }
}

void MedicalSystem::findRouteToHospital(string fromNodeID, string hospitalID) {
    cout << "\n--- Finding Route to Hospital ---\n";

    Hospital* h = hospitalHash->search(hospitalID);
    if (h == nullptr) {
        cout << "Hospital not found!\n";
        return;
    }

    cout << "Finding route to: " << h->name << " (" << h->sector << ")\n";

    cityGraph->dijkstraAlgo(fromNodeID, hospitalID, "ANY");
}

// ============ PHARMACY MANAGEMENT ============

void MedicalSystem::registerPharmacy(string id, string name, string sector,
    double lat, double lon,
    Medicine initialMed) {
    cout << "Registering pharmacy: " << name << " (" << id << ")...\n";

    pharmacy* newPharm = new pharmacy(id, name, sector, lat, lon, initialMed, 20);

    PharmacyNode* node = new PharmacyNode(newPharm);
    node->next = pharmacyHead;
    pharmacyHead = node;
    pharmacyCount++;

    Node pharmacyNode(id, name, "PHARMACY", lat, lon);
    cityGraph->addNode(pharmacyNode);

    cityGraph->connectToNearbyNodes(id, 1.5, "BUS_STOP");

    cout << "Pharmacy " << name << " registered successfully!\n";
}

void MedicalSystem::registerPharmacyEmpty(string id, string name, string sector,
    double lat, double lon) {
    cout << "Registering pharmacy: " << name << " (" << id << ")...\n";

    pharmacy* newPharm = new pharmacy(id, name, sector, lat, lon, 20);

    PharmacyNode* node = new PharmacyNode(newPharm);
    node->next = pharmacyHead;
    pharmacyHead = node;
    pharmacyCount++;

    Node pharmacyNode(id, name, "PHARMACY", lat, lon);
    cityGraph->addNode(pharmacyNode);

    cityGraph->connectToNearbyNodes(id, 1.5, "BUS_STOP");
    cityGraph->connectToNearbyNodes(id, 1.5, "HOSPITAL");

    cout << "Pharmacy " << name << " registered (empty inventory).\n";
}

void MedicalSystem::removePharmacy(string pharmacyID) {
    cout << "\n--- Removing Pharmacy: " << pharmacyID << " ---\n";

    PharmacyNode* prev = nullptr;
    PharmacyNode* curr = pharmacyHead;

    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->pharm->getID(), pharmacyID)) {
            string pharmacyName = curr->pharm->getName();

            if (prev == nullptr) {
                pharmacyHead = curr->next;
            }
            else {
                prev->next = curr->next;
            }

            pharmacyCount--;
            delete curr->pharm;
            delete curr;

            cout << "Removed pharmacy: " << pharmacyName << "\n";
            return;
        }

        prev = curr;
        curr = curr->next;
    }

    cout << "Pharmacy " << pharmacyID << " not found.\n";
}

void MedicalSystem::displayAllPharmacies() {
    cout << "\n========== ALL PHARMACIES ==========\n";

    if (pharmacyHead == nullptr) {
        cout << "No pharmacies registered.\n";
        return;
    }

    PharmacyNode* curr = pharmacyHead;
    int count = 1;

    while (curr != nullptr) {
        cout << "\n[" << count++ << "] ";
        curr->pharm->displayPharmacyDetails();
        curr = curr->next;
    }

    cout << "\nTotal Pharmacies: " << pharmacyCount << "\n";
    cout << "====================================\n";
}

void MedicalSystem::viewPharmacy(string pharmacyName) {
    cout << "\n--- Viewing Pharmacy: " << pharmacyName << " ---\n";

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->pharm->getName(), pharmacyName)) {
            curr->pharm->displayPharmacyDetails();
            return;
        }
        curr = curr->next;
    }

    cout << "Pharmacy " << pharmacyName << " not found.\n";
}

void MedicalSystem::viewPharmacyByID(string pharmacyID) {
    cout << "\n--- Viewing Pharmacy ID: " << pharmacyID << " ---\n";

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->pharm->getID(), pharmacyID)) {
            curr->pharm->displayPharmacyDetails();
            return;
        }
        curr = curr->next;
    }

    cout << "Pharmacy " << pharmacyID << " not found.\n";
}

void MedicalSystem::addMedicineToPharmacy(string pharmacyID, string medName,
    string formula, int price) {
    cout << "\n--- Adding Medicine to Pharmacy ---\n";

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->pharm->getID(), pharmacyID)) {
            Medicine med(medName, formula, price);
            curr->pharm->addMedicine(med);
            cout << "Added " << medName << " to " << curr->pharm->getName() << "\n";
            return;
        }
        curr = curr->next;
    }

    cout << "Pharmacy " << pharmacyID << " not found.\n";
}

void MedicalSystem::removeMedicineFromPharmacy(string pharmacyID, string medicineName) {
    cout << "\n--- Removing Medicine from Pharmacy ---\n";

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->pharm->getID(), pharmacyID)) {
            bool removed = curr->pharm->removeMedicine(medicineName);
            if (removed) {
                cout << "Removed " << medicineName
                    << " from " << curr->pharm->getName() << "\n";
            }
            else {
                cout << "Medicine " << medicineName << " not found.\n";
            }
            return;
        }
        curr = curr->next;
    }

    cout << "Pharmacy " << pharmacyID << " not found.\n";
}

int MedicalSystem::getPharmacyCount() {
    return pharmacyCount;
}

// ============ MEDICINE SEARCH & PRICING ============

void MedicalSystem::searchMedicine(string medicineName) {
    cout << "\n========== SEARCHING FOR MEDICINE ==========\n";
    cout << "Medicine: " << medicineName << "\n";
    cout << "============================================\n";

    bool found = false;
    int foundCount = 0;

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        Medicine* med = curr->pharm->searchByName(medicineName);
        if (med != nullptr) {
            foundCount++;
            cout << "\n[" << foundCount << "] Found at: "
                << curr->pharm->getName() << "\n";
            cout << " Sector: " << curr->pharm->getSector() << "\n";
            cout << " Medicine: " << med->medicineName << "\n";
            cout << " Formula: " << med->formula << "\n";
            cout << " Price: Rs. " << med->price << "\n";
            found = true;
        }
        curr = curr->next;
    }

    cout << "\n============================================\n";
    if (!found) {
        cout << "Medicine '" << medicineName << "' not found.\n";
    }
    else {
        cout << "Total: Found at " << foundCount << " pharmacy/pharmacies.\n";
    }
}

void MedicalSystem::searchMedicineByFormula(string formula) {
    cout << "\n========== SEARCHING BY FORMULA ==========\n";
    cout << "Formula: " << formula << "\n";
    cout << "==========================================\n";

    bool found = false;
    int foundCount = 0;

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        Medicine* med = curr->pharm->searchByFormula(formula);
        if (med != nullptr) {
            foundCount++;
            cout << "\n[" << foundCount << "] Found at: "
                << curr->pharm->getName() << "\n";
            cout << " Sector: " << curr->pharm->getSector() << "\n";
            cout << " Medicine: " << med->medicineName << "\n";
            cout << " Price: Rs. " << med->price << "\n";
            found = true;
        }
        curr = curr->next;
    }

    if (!found) {
        cout << "No medicine with formula '" << formula << "' found.\n";
    }
    else {
        cout << "Total: Found at " << foundCount << " pharmacy/pharmacies.\n";
    }
}

void MedicalSystem::findCheapestPharmacyForMedicine(string medicineName) {
    cout << "\n--- Finding Cheapest Pharmacy for: "
        << medicineName << " ---\n";

    pharmacy* cheapestPharm = nullptr;
    Medicine* cheapestMed = nullptr;
    int minPrice = -1;

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        Medicine* med = curr->pharm->searchByName(medicineName);
        if (med != nullptr) {
            if (minPrice < 0 || med->price < minPrice) {
                minPrice = med->price;
                cheapestPharm = curr->pharm;
                cheapestMed = med;
            }
        }
        curr = curr->next;
    }

    if (cheapestPharm != nullptr) {
        cout << "Cheapest at: " << cheapestPharm->getName() << "\n";
        cout << " Sector: " << cheapestPharm->getSector() << "\n";
        cout << " Price: Rs. " << cheapestMed->price << "\n";
    }
    else {
        cout << "Medicine '" << medicineName << "' not found.\n";
    }
}

pharmacy* MedicalSystem::findNearestPharmacy(double lat, double lon) {
    cout << "\n--- Finding Nearest Pharmacy to (" << lat
        << ", " << lon << ") ---\n";

    string nearestID = cityGraph->findNearestNode(lat, lon, "PHARMACY");
    if (nearestID.empty()) {
        cout << "No pharmacies found in system.\n";
        return nullptr;
    }

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->pharm->getID(), nearestID)) {
            double dist = calculateDistance(
                lat, lon,
                curr->pharm->getLatitude(),
                curr->pharm->getLongitude()
            );
            cout << "Nearest pharmacy: " << curr->pharm->getName() << "\n";
            cout << " Distance: " << dist << " km\n";
            cout << " Sector: " << curr->pharm->getSector() << "\n";
            return curr->pharm;
        }
        curr = curr->next;
    }

    cout << "Pharmacy data not found.\n";
    return nullptr;
}

pharmacy* MedicalSystem::findNearestPharmacyWithMedicine(double lat, double lon,
    string medicineName) {
    cout << "\n--- Finding Nearest Pharmacy with " << medicineName << " ---\n";
    cout << "Location: (" << lat << ", " << lon << ")\n";

    pharmacy* nearest = nullptr;
    double minDistance = -1.0;

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        Medicine* med = curr->pharm->searchByName(medicineName);
        if (med != nullptr) {
            double dist = calculateDistance(
                lat, lon,
                curr->pharm->getLatitude(),
                curr->pharm->getLongitude()
            );
            if (minDistance < 0 || dist < minDistance) {
                minDistance = dist;
                nearest = curr->pharm;
            }
        }
        curr = curr->next;
    }

    if (nearest != nullptr) {
        cout << "Found: " << nearest->getName() << "\n";
        cout << " Distance: " << minDistance << " km\n";
        cout << " Sector: " << nearest->getSector() << "\n";
        Medicine* med = nearest->searchByName(medicineName);
        if (med) {
            cout << " Price: Rs. " << med->price << "\n";
        }
    }
    else {
        cout << "No pharmacy with " << medicineName << " found.\n";
    }

    return nearest;
}

void MedicalSystem::compareMedicinePrices(string medicineName) {
    cout << "\n========== PRICE COMPARISON ==========\n";
    cout << "Medicine: " << medicineName << "\n";
    cout << "======================================\n";

    const int MAX_PHARM = 50;
    pharmacy* pharmacies[MAX_PHARM];
    int prices[MAX_PHARM];
    int count = 0;

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr && count < MAX_PHARM) {
        Medicine* med = curr->pharm->searchByName(medicineName);
        if (med != nullptr) {
            pharmacies[count] = curr->pharm;
            prices[count] = med->price;
            count++;
        }
        curr = curr->next;
    }

    if (count == 0) {
        cout << "Medicine not found at any pharmacy.\n";
        return;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (prices[j] > prices[j + 1]) {
                int tempPrice = prices[j];
                prices[j] = prices[j + 1];
                prices[j + 1] = tempPrice;

                pharmacy* tempPharm = pharmacies[j];
                pharmacies[j] = pharmacies[j + 1];
                pharmacies[j + 1] = tempPharm;
            }
        }
    }

    cout << "\nFound at " << count << " pharmacy/pharmacies:\n\n";
    for (int i = 0; i < count; i++) {
        cout << (i + 1) << ". " << pharmacies[i]->getName()
            << " (" << pharmacies[i]->getSector() << ")\n";
        cout << " Price: Rs. " << prices[i];
        if (i == 0) cout << " <-- Cheapest";
        if (i == count - 1 && count > 1) cout << " (Most Expensive)";
        cout << "\n\n";
    }

    if (count > 1) {
        int savings = prices[count - 1] - prices[0];
        cout << "Max savings: Rs. " << savings << "\n";
    }
}

void MedicalSystem::getTopNearestPharmacies(double lat, double lon, int topN) {
    cout << "\n--- Top " << topN << " Nearest Pharmacies ---\n";
    cout << "Location: (" << lat << ", " << lon << ")\n\n";

    const int MAX_PHARM = 50;
    pharmacy* pharmacyList[MAX_PHARM];
    double distances[MAX_PHARM];
    int count = 0;

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr && count < MAX_PHARM) {
        pharmacyList[count] = curr->pharm;
        distances[count] = calculateDistance(
            lat, lon,
            curr->pharm->getLatitude(),
            curr->pharm->getLongitude()
        );
        count++;
        curr = curr->next;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (distances[j] > distances[j + 1]) {
                double tempDist = distances[j];
                distances[j] = distances[j + 1];
                distances[j + 1] = tempDist;

                pharmacy* tempPharm = pharmacyList[j];
                pharmacyList[j] = pharmacyList[j + 1];
                pharmacyList[j + 1] = tempPharm;
            }
        }
    }

    int limit = (topN < count) ? topN : count;

    for (int i = 0; i < limit; i++) {
        cout << (i + 1) << ". " << pharmacyList[i]->getName() << "\n";
        cout << " Distance: " << distances[i] << " km\n";
        cout << " Sector: " << pharmacyList[i]->getSector() << "\n\n";
    }
}

void MedicalSystem::findRouteToPharmacy(string fromNodeID, string pharmacyID) {
    cout << "\n--- Finding Route to Pharmacy ---\n";

    PharmacyNode* curr = pharmacyHead;
    while (curr != nullptr) {
        if (equalsIgnoreCase(curr->pharm->getID(), pharmacyID)) {
            cout << "Finding route to: " << curr->pharm->getName() << "\n";
            cityGraph->dijkstraAlgo(fromNodeID, pharmacyID, "ANY");
            return;
        }
        curr = curr->next;
    }

    cout << "Pharmacy not found!\n";
}

// ============ MENU SYSTEM ============

void MedicalSystem::displayMenu() {
    cout << endl << "========================================" << endl;
    cout << " SMART CITY MEDICAL SYSTEM" << endl;
    cout << "========================================" << endl;
    cout << "--- Hospital Management ---" << endl;
    cout << "1. Register New Hospital" << endl;
    cout << "2. View Hospital Details" << endl;
    cout << "3. Display All Hospitals" << endl;
    cout << "4. Deactivate Hospital" << endl;
    cout << "--- Doctor Management ---" << endl;
    cout << "5. Hire Doctor" << endl;
    cout << "6. Fire Doctor" << endl;
    cout << "7. View Doctors by Specialization" << endl;
    cout << "--- Patient Management ---" << endl;
    cout << "8. Admit Patient" << endl;
    cout << "9. Discharge Patient" << endl;
    cout << "10. View Patients at Hospital" << endl;
    cout << "11. Transfer Patient" << endl;
    cout << "--- Emergency Beds ---" << endl;
    cout << "12. View Hospital with Most Beds" << endl;
    cout << "13. View All Emergency Beds" << endl;
    cout << "14. Reserve/Release Bed" << endl;
    cout << "--- Nearest Hospital ---" << endl;
    cout << "15. Find Nearest Hospital" << endl;
    cout << "16. Find Route to Hospital" << endl;
    cout << "--- Pharmacy & Medicine ---" << endl;
    cout << "17. Display All Pharmacies" << endl;
    cout << "18. Search Medicine" << endl;
    cout << "19. Find Nearest Pharmacy" << endl;
    cout << "20. Compare Medicine Prices" << endl;
    cout << "21. Find Route to Pharmacy" << endl;
    cout << "23. Search Medicine by Formula" << endl;
    cout << "--- Network ---" << endl;
    cout << "22. Display Medical Network" << endl;
    cout << "0. Exit" << endl;
    cout << "========================================" << endl;
    cout << "Enter choice: ";
}

void MedicalSystem::run() {
    int choice;
    while (true) {
        displayMenu();
        cin >> choice;
        cin.ignore();

        if (choice == 0) {
            cout << "Exiting Medical System..." << endl;
            break;
        }

        switch (choice) {
        case 1: {
            string id, name, sector, spec;
            int beds;
            double lat, lon;

            cout << "Enter Hospital ID: ";
            getline(cin, id);
            cout << "Enter Name: ";
            getline(cin, name);
            cout << "Enter Sector: ";
            getline(cin, sector);
            cout << "Enter Emergency Beds: ";
            cin >> beds;
            cin.ignore();
            cout << "Enter Specialization: ";
            getline(cin, spec);
            cout << "Enter Latitude: ";
            cin >> lat;
            cout << "Enter Longitude: ";
            cin >> lon;
            cin.ignore();

            registerHospital(id, name, sector, beds, spec, lat, lon);
            break;
        }
        case 2: {
            string id;
            cout << "Enter Hospital ID: ";
            getline(cin, id);
            viewHospital(id);
            break;
        }
        case 3: {
            displayAllHospitals();
            break;
        }
        case 4: {
            string id;
            cout << "Enter Hospital ID to deactivate: ";
            getline(cin, id);
            deactivateHospital(id);
            break;
        }
        case 5: {
            string hospID, docName, spec;
            cout << "Enter Hospital ID: ";
            getline(cin, hospID);
            cout << "Enter Doctor Name: ";
            getline(cin, docName);
            cout << "Enter Specialization: ";
            getline(cin, spec);
            hireDoctor(hospID, docName, spec);
            break;
        }
        case 6: {
            string hospID, docName;
            cout << "Enter Hospital ID: ";
            getline(cin, hospID);
            cout << "Enter Doctor Name: ";
            getline(cin, docName);
            fireDoctor(hospID, docName);
            break;
        }
        case 7: {
            string spec;
            cout << "Enter Specialization: ";
            getline(cin, spec);
            viewDoctorsBySpecialization(spec);
            break;
        }
        case 8: {
            string hospID, patName, condition;
            int age;
            cout << "Enter Hospital ID: ";
            getline(cin, hospID);
            cout << "Enter Patient Name: ";
            getline(cin, patName);
            cout << "Enter Age: ";
            cin >> age;
            cin.ignore();
            cout << "Enter Condition: ";
            getline(cin, condition);
            admitPatient(hospID, patName, age, condition);
            break;
        }
        case 9: {
            string hospID, patName;
            cout << "Enter Hospital ID: ";
            getline(cin, hospID);
            cout << "Enter Patient Name: ";
            getline(cin, patName);
            dischargePatient(hospID, patName);
            break;
        }
        case 10: {
            string hospID;
            cout << "Enter Hospital ID: ";
            getline(cin, hospID);
            viewPatientsAtHospital(hospID);
            break;
        }
        case 11: {
            string fromID, toID, patName;
            cout << "Enter Source Hospital ID: ";
            getline(cin, fromID);
            cout << "Enter Destination Hospital ID: ";
            getline(cin, toID);
            cout << "Enter Patient Name: ";
            getline(cin, patName);
            transferPatient(fromID, toID, patName);
            break;
        }
        case 12: {
            viewMaxEmergencyBeds();
            break;
        }
        case 13: {
            viewAllEmergencyBeds();
            break;
        }
        case 14: {
            string hospID;
            int action;
            cout << "Enter Hospital ID: ";
            getline(cin, hospID);
            cout << "1. Reserve Bed 2. Release Bed: ";
            cin >> action;
            cin.ignore();
            if (action == 1) reserveEmergencyBed(hospID);
            else releaseEmergencyBed(hospID);
            break;
        }
        case 15: {
            double lat, lon;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cin.ignore();
            findNearestHospital(lat, lon);
            break;
        }
        case 16: {
            string fromID, hospID;
            cout << "Enter your current Node ID: ";
            getline(cin, fromID);
            cout << "Enter Hospital ID: ";
            getline(cin, hospID);
            findRouteToHospital(fromID, hospID);
            break;
        }
        case 17: {
            displayAllPharmacies();
            break;
        }
        case 18: {
            string medName;
            cout << "Enter Medicine Name: ";
            getline(cin, medName);
            searchMedicine(medName);
            break;
        }
        case 19: {
            double lat, lon;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cin.ignore();
            findNearestPharmacy(lat, lon);
            break;
        }
        case 20: {
            string medName;
            cout << "Enter Medicine Name: ";
            getline(cin, medName);
            compareMedicinePrices(medName);
            break;
        }
        case 21: {
            string fromID, pharmID;
            cout << "Enter your current Node ID: ";
            getline(cin, fromID);
            cout << "Enter Pharmacy ID: ";
            getline(cin, pharmID);
            findRouteToPharmacy(fromID, pharmID);
            break;
        }
        case 22: {
            displayNetwork();
            break;
        }
        case 23: {
            string formula;
            cout << "Enter Medicine Formula: ";
            getline(cin, formula);
            searchMedicineByFormula(formula);
            break;
        }
        default:
            cout << "Invalid choice! Try again." << endl;
        }
    }
}
