#define _CRT_SECURE_NO_WARNINGS

#include "TransportSystem.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <thread>
#include <chrono>
using namespace std;

const double PI = 3.14159265358979323846;
const double EARTH_RADIUS = 6371.0; // km

// StopQueueNode Implementation
StopQueueNode::StopQueueNode(string id, CircularQueue* q) : stopID(id), queue(q), next(nullptr) {}

TransportSystem::TransportSystem(Graph* sharedGraph) : stopQueues(nullptr), nextTicketNumber(1001) {
    if (sharedGraph) {
        cityGraph = sharedGraph;
        ownsGraph = false;
    }
    else {
        cityGraph = new Graph();
        ownsGraph = true;
    }
    busHash = new BusHashTable(101);
}

TransportSystem::~TransportSystem() {
    if (ownsGraph) {
        delete cityGraph;
    }
    delete busHash;

    StopQueueNode* current = stopQueues;
    while (current) {
        StopQueueNode* temp = current;
        current = current->next;
        delete temp->queue;
        delete temp;
    }
}

bool TransportSystem::isValidString(string str) {
    for (int i = 0; i < str.size(); i++) {
        if (!isspace(str[i]))
            return true;
    }
    return false;
}

bool TransportSystem::isValidCoordinate(double lat, double lon) {
    if (lat < -90.0 || lat > 90.0) {
        cout << "Invalid latitude! Must be between -90 and +90." << endl;
        return false;
    }
    if (lon < -180.0 || lon > 180.0) {
        cout << "Invalid longitude! Must be between -180 and +180." << endl;
        return false;
    }
    return true;
}

Graph* TransportSystem::getCityGraph() {
    return cityGraph;
}

string TransportSystem::cleanString(string str) {
    size_t first = str.find_first_not_of(" \t\r\n"); // Remove leading whitespace

    if (first == string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\r\n"); // Remove trailing whitespace
    
    return str.substr(first, last - first + 1);
}

void TransportSystem::parseCoordinates(string coordStr, double& lat, double& lon) {
    lat = 0.0;
    lon = 0.0;

    if (!coordStr.empty() && coordStr[0] == '"') { // Remove quotation marks
        coordStr = coordStr.substr(1, coordStr.length() - 2);
    }

    size_t commaPos = coordStr.find(',');
    if (commaPos != string::npos) {
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

void TransportSystem::parseRoute(string routeStr, Bus* bus) {
    string stop = "";

    for (size_t i = 0; i < routeStr.length(); i++) {
        if (routeStr[i] == '>') {
            stop = cleanString(stop);
            if (!stop.empty()) {
                bus->addStopToRoute(stop);
            }
            stop = "";
        }
        else {
            stop += routeStr[i];
        }
    }

    // Add last stop
    stop = cleanString(stop);
    if (!stop.empty()) {
        bus->addStopToRoute(stop);
    }
}

double TransportSystem::calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Convert degree difference into radians
    double diffLat = (lat2 - lat1) * PI / 180.0;
    double diffLon = (lon2 - lon1) * PI / 180.0;

    // Convert latitude to radians
    lat1 = lat1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;

    // Haversine formula
    double a = sin(diffLat / 2) * sin(diffLat / 2) + sin(diffLon / 2) * sin(diffLon / 2) * cos(lat1) * cos(lat2);

    // Convert to angle (C = 2 * arcsin( sqrt(a) )), but safer version
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    // Convert angle to distance (S = r * theta)
    return EARTH_RADIUS * c;
}

bool TransportSystem::parseStopsFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); // Skip header

    int lineNum = 1;
    while (getline(file, line)) {
        lineNum++;
        if (line.empty()) continue;

        string stopID = "";
        string name = "";
        string coordStr = "";

        int fieldCount = 0;
        bool inQuotes = false;
        string currentField = "";

        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];

            if (c == '"') {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes) { // Coordinates contain comma between them
                if (fieldCount == 0) {
                    stopID = cleanString(currentField);
                }
                else if (fieldCount == 1) {
                    name = cleanString(currentField);
                }
                currentField = "";
                fieldCount++;
            }
            else {
                currentField += c;
            }
        }

        coordStr = cleanString(currentField);

        double lat = 0.0, lon = 0.0;
        parseCoordinates(coordStr, lat, lon);

        cout << "Line " << lineNum << ": " << stopID << " -> (" << lat << ", " << lon << ")" << endl;

        addBusStop(stopID, name, lat, lon);
    }

    file.close();
    cout << "Loaded " << (lineNum - 1) << " stops from " << filename << endl;
    return true;
}

bool TransportSystem::parseBusesFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        string busNo = "";
        string company = "";
        string currentStop = "";
        string route = "";

        int fieldCount = 0;
        string currentField = "";

        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];
            if (c == ',') {
                if (fieldCount == 0) {
                    busNo = cleanString(currentField);
                }
                else if (fieldCount == 1) {
                    company = cleanString(currentField);
                }
                else if (fieldCount == 2) {
                    currentStop = cleanString(currentField);
                }
                currentField = "";
                fieldCount++;
            }
            else {
                currentField += c;
            }
        }

        route = cleanString(currentField);
        Bus* bus = new Bus(busNo, company, currentStop);
        parseRoute(route, bus);

        addBusWithRoute(bus);
    }

    file.close();
    cout << "Loaded buses from " << filename << endl;
    return true;
}

void TransportSystem::loadDataFromFiles(string stopsFile, string busesFile) {
    cout << endl << "===== Loading Transport Data =====" << endl;

    bool stopsLoaded = parseStopsFile(stopsFile);
    bool busesLoaded = parseBusesFile(busesFile);

    if (stopsLoaded && busesLoaded) {
        cout << " Transport system initialized successfully!" << endl;
    }
    else {
        cout << "  Warning: Some data files could not be loaded." << endl;
        if (!stopsLoaded) cout << "   - Stops file: FAILED" << endl;
        if (!busesLoaded) cout << "   - Buses file: FAILED" << endl;
    }
}

void TransportSystem::registerTransportCompany(string companyName) {
    cout << "Transport company '" << companyName << "' registered successfully!" << endl;
}

void TransportSystem::registerBus(string busNo, string company, string currentStop) {
    Bus* bus = new Bus(busNo, company, currentStop);
    busHash->insert(busNo, bus);
    cout << "Bus " << busNo << " registered successfully!" << endl;
}

void TransportSystem::addBusWithRoute(Bus* bus) {
    busHash->insert(bus->getBusNo(), bus);

    // Connect stops in the route (+ calculate distances)
    StringLinkedList* route = bus->getRoute();
    StringNode* current = route->getHead();

    while (current && current->next) {
        string stop1 = current->data;
        string stop2 = current->next->data;

        Node* busStop1 = cityGraph->getNode(stop1);
        Node* busStop2 = cityGraph->getNode(stop2);

        if (busStop1 && busStop2) {
            double distance = calculateDistance(busStop1->getLatitude(), busStop1->getLongitude(), busStop2->getLatitude(), busStop2->getLongitude());

            // Add edge in bidirection
            cityGraph->addEdge(stop1, stop2, distance);
            cityGraph->addEdge(stop2, stop1, distance);
        }

        current = current->next;
    }
}

Bus* TransportSystem::getBusByNumber(string busNo) {
    return busHash->search(busNo);
}

void TransportSystem::updateBusLocation(string busNo, string newStop) {
    Bus* bus = busHash->search(busNo);

    if (bus) {
        if (bus->isStopInRoute(newStop)) {
            bus->setCurrentStop(newStop);
            cout << "Bus " << busNo << " location updated to " << newStop << endl;
        }
        else {
            cout << "Stop " << newStop << " is not in bus route!" << endl;
         }
    }
    else {
        cout << "Bus " << busNo << " not found!" << endl;
    }
}

void TransportSystem::displayAllBuses() {
    cout << endl << "===== All Buses =====" << endl;
    busHash->display();
}

void TransportSystem::addBusStop(string stopID, string name, double lat, double lon) {
    if (!isValidCoordinate(lat, lon)) {
        return;
    }

    if (stopID.empty() || name.empty()) {
        cout << "Stop ID and name cannot be empty!" << endl;
        return;
    }

    Node stop(stopID, name, "BUS_STOP", lat, lon);
    cityGraph->addNode(stop);
}

void TransportSystem::connectStops(string stop1, string stop2) {
    Node* busStop1 = cityGraph->getNode(stop1);
    Node* busStop2 = cityGraph->getNode(stop2);

    if (busStop1 && busStop2) {
        double distance = calculateDistance(busStop1->getLatitude(), busStop1->getLongitude(), busStop2->getLatitude(), busStop2->getLongitude());

        cityGraph->addEdge(stop1, stop2, distance);
        cityGraph->addEdge(stop2, stop1, distance);
        cout << "Connected " << stop1 << " to " << stop2 << " (Distance: " << distance << " km)" << endl;
    }
    else {
        cout << "Stops not found!" << endl;
    }
}

//for school bus registration 
void TransportSystem::registerSchoolBus(string schoolID, string busNo, string company) {
    if (!cityGraph->hasNode(schoolID)) {
        cout << "School ID '" << schoolID << "' not found!" << endl;
        return;
    }

    string houseIDs[10];
    int houseCount = cityGraph->getNearByHouses(schoolID, houseIDs, 10, 3.0); // 3km radius
    if (houseCount == 0) {
        cout << "No houses found near school ID '" << schoolID << "'!" << endl;
        return;
    }

    string routeStr = "";
    for (int i = 0; i < houseCount; i++) {
        routeStr += houseIDs[i];
        if (i < houseCount - 1) {
            routeStr += " > ";
        }
    }

    Bus* bus = new Bus(busNo, company, houseIDs[0]);
    parseRoute(routeStr, bus);
    addBusWithRoute(bus);        // connects and inserts into hash

    cout << "School bus " << busNo << " created for school " << " with " << houseCount << " house stops." << endl;

}

void TransportSystem::displayNetwork() {
    cout << endl << "========== Transport Sector Graph ==========" << endl;
    cityGraph->displayGraph("BUS_STOP");
}

void TransportSystem::findShortestPath(string source, string destination) {
    cityGraph->dijkstraAlgo(source, destination, "BUS_STOP");
}

void TransportSystem::searchNearestBus(double passengerLat, double passengerLon) {
    cout << endl << "===== Finding Nearest Bus =====" << endl;

    string nearestStop = cityGraph->findNearestNode(passengerLat, passengerLon, "BUS_STOP");

    if (nearestStop.empty()) {
        cout << "No stops found in the network!" << endl;
        return;
    }

    Node* stop = cityGraph->getNode(nearestStop);
    double distance = calculateDistance(passengerLat, passengerLon, stop->getLatitude(), stop->getLongitude());

    cout << "Nearest stop: " << nearestStop << " (" << stop->getName() << ")" << endl;
    cout << "Distance: " << distance << " km" << endl;

    // Find buses that have this stop in their route
    cout << endl << "Buses at or near this stop:" << endl;
    bool found = false;

    int busCount = 0;
    Bus** allBuses = busHash->getAllBuses(busCount);

    if (!allBuses) {
        cout << "No buses registered in the system." << endl;
        return;
    }

    for (int i = 0; i < busCount; i++) {
        if (allBuses[i]->isStopInRoute(nearestStop)) {
            cout << "  - Bus " << allBuses[i]->getBusNo() << " (" << allBuses[i]->getCompany() << ")" << " - Currently at: " << allBuses[i]->getCurrentStop() << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No buses pass through this stop." << endl;
    }

    delete[] allBuses;
}

string TransportSystem::findNearestStop(double lat, double lon) {
    return cityGraph->findNearestNode(lat, lon, "BUS_STOP");
}

// Helper function to get or create new queue for a stop
CircularQueue* TransportSystem::getOrCreateQueue(string stopID) {
    StopQueueNode* current = stopQueues;

    // Search for existing queue
    while (current) {
        if (current->stopID == stopID) {
            return current->queue;
        }
        current = current->next;
    }

    // Create new queue if not found
    CircularQueue* newQueue = new CircularQueue(50); // Store 50 passengers
    StopQueueNode* newNode = new StopQueueNode(stopID, newQueue);
    newNode->next = stopQueues;
    stopQueues = newNode;

    return newQueue;
}

// Get current time as string
string TransportSystem::getCurrentTime() {
    time_t currentTime = time(0);

    char buffer[80];
    struct tm* timeinfo = localtime(&currentTime);
    
    strftime(buffer, 80, "%H:%M:%S", timeinfo);

    return string(buffer);
}

// ==== Circular Queue Operations ====
void TransportSystem::addPassengerToQueue(string stopID, string name, string destination) {
    if (!cityGraph->hasNode(stopID)) {
        cout << "Stop '" << stopID << "' not found!" << endl;
        return;
    }

    CircularQueue* queue = getOrCreateQueue(stopID);
    Passenger p(name, destination, nextTicketNumber++);

    if (queue->enqueue(p)) {
        cout << "Passenger added to queue at " << stopID << endl;
        cout << "Ticket Number: " << p.getTicketNumber() << endl;
    }
}

void TransportSystem::boardPassengers(string busNo, string stopID, int count) {
    if (count <= 0) {
        cout << "Invalid passenger count! Must be positive." << endl;
        return;
    }

    if (count > 100) {
        cout << "Cannot board more than 100 passengers at once!" << endl;
        return;
    }
    
    Bus* bus = busHash->search(busNo);
    if (!bus) {
        cout << "Bus '" << busNo << "' not found!" << endl;
        return;
    }

    if (bus->getCurrentStop() != stopID) {
        cout << "Bus is not at stop '" << stopID << "'!" << endl;
        cout << "Current location: " << bus->getCurrentStop() << endl;
        return;
    }

    CircularQueue* queue = getOrCreateQueue(stopID);

    if (queue->isEmpty()) {
        cout << "No passengers waiting at " << stopID << endl;
        return;
    }

    cout << endl << "===== Boarding Passengers =====" << endl;
    cout << "Bus: " << busNo << " at " << stopID << endl;

    int boarded = 0;
    for (int i = 0; i < count && !queue->isEmpty(); i++) {
        Passenger p = queue->dequeue();
        cout << "Boarded: ";
        p.display();
        boarded++;
    }

    cout << "Total passengers boarded: " << boarded << endl;
    cout << "Remaining in queue: " << queue->size() << endl;
}

void TransportSystem::displayQueueAtStop(string stopID) {
    if (!cityGraph->hasNode(stopID)) {
        cout << "Stop '" << stopID << "' not found!" << endl;
        return;
    }

    CircularQueue* queue = getOrCreateQueue(stopID);
    Node* stop = cityGraph->getNode(stopID);

    cout << endl << "===== Passenger Queue at " << stop->getName() << " (" << stopID << ") =====" << endl;
    queue->display();
}

void TransportSystem::simulatePassengerArrival(string stopID) {
    if (!cityGraph->hasNode(stopID)) {
        cout << "Stop '" << stopID << "' not found!" << endl;
        return;
    }

    string names[] = { "Ali", "Fatima", "Ahmed", "Taimoor", "Hassan", "Asad", "Usman", "Zainab" };
    string destinations[] = { "Stop1", "Stop2", "Stop3", "Stop4", "Stop5", "Stop8" };

    int numPassengers = (rand() % 5) + 1; // 1-5 passengers

    cout << endl << "===== Simulating Passenger Arrivals at " << stopID << " =====" << endl;
    for (int i = 0; i < numPassengers; i++) {
        string name = names[rand() % 8];        // random name
        string dest = destinations[rand() % 6]; // random destination
        addPassengerToQueue(stopID, name, dest);
    }
    cout << "Added " << numPassengers << " passengers to queue." << endl;
}

// ==== Stack Implementation ====
void TransportSystem::recordBusStop(string busNo, string stopID) {
    Bus* bus = busHash->search(busNo);
    if (!bus) {
        cout << "Bus '" << busNo << "' not found!" << endl;
        return;
    }

    Node* stop = cityGraph->getNode(stopID);
    if (!stop) {
        cout << "Stop '" << stopID << "' not found!" << endl;
        return;
    }

    string timestamp = getCurrentTime();
    bus->recordStopVisit(stopID, stop->getName(), timestamp);
    bus->setCurrentStop(stopID);

    cout << "Recorded: Bus " << busNo << " visited " << stopID << " at " << timestamp << endl;
}

void TransportSystem::displayBusHistory(string busNo) {
    Bus* bus = busHash->search(busNo);
    if (!bus) {
        cout << "Bus '" << busNo << "' not found!" << endl;
        return;
    }

    bus->displayTravelHistory();
}

// ==== Airport/Rail Integration ====
void TransportSystem::addAirportStop(string stopID, string name, double lat, double lon) {
    Node airportStop(stopID, name, "AIRPORT", lat, lon);
    cityGraph->addNode(airportStop);

    // Connect to nearby bus stops (Within 2km)
    cityGraph->connectToNearbyNodes(stopID, 2.0, "BUS_STOP");

    cout << "Airport stop '" << name << "' added and connected to transport network!" << endl;
}

void TransportSystem::addRailwayStop(string stopID, string name, double lat, double lon) {
    Node railStop(stopID, name, "RAILWAY", lat, lon);
    cityGraph->addNode(railStop);

    // Connect to nearby bus stops (Within 2km)
    cityGraph->connectToNearbyNodes(stopID, 2.0, "BUS_STOP");

    cout << "Railway stop '" << name << "' added and connected to transport network!" << endl;
}

void TransportSystem::findRouteToAirport(string fromStop) {
    cout << endl << "===== Finding Route to Airport =====" << endl;

    // Find nearest airport
    Node* from = cityGraph->getNode(fromStop);
    if (!from) {
        cout << "Starting stop not found!" << endl;
        return;
    }

    string nearestAirport = cityGraph->findNearestNode(from->getLatitude(), from->getLongitude(), "AIRPORT");

    if (nearestAirport.empty()) {
        cout << "No airport found in the network!" << endl;
        return;
    }

    cout << "Nearest airport: " << nearestAirport << endl;
    cityGraph->dijkstraAlgo(fromStop, nearestAirport, "ANY");
}

void TransportSystem::displaySpecialStops() {
    cout << endl << "===== Special Transport Hubs =====" << endl;
    cout << "Airports and Railway Stations in the network:" << endl << endl;

    cityGraph->displayNodesByType("AIRPORT");
    cityGraph->displayNodesByType("RAILWAY");
}

// ==== Real-Time Simulation ====
void TransportSystem::simulateBusMovement(string busNo, int delaySeconds) {
    Bus* bus = busHash->search(busNo);
    if (!bus) {
        cout << "Bus '" << busNo << "' not found!" << endl;
        return;
    }

    StringLinkedList* route = bus->getRoute();
    if (route->isEmpty()) {
        cout << "Bus has no route!" << endl;
        return;
    }

    cout << endl << "===== Real-time Simulation: Bus " << busNo << " =====" << endl;
    cout << "Company: " << bus->getCompany() << endl;
    cout << "Route: ";
    route->display();
    cout << endl << "Starting simulation (with delay: " << delaySeconds << "s per stop)..." << endl << endl;

    StringNode* current = route->getHead();
    int stopNumber = 1;

    while (current) {
        string stopID = current->data;
        Node* stop = cityGraph->getNode(stopID);

        if (stop) {
            cout << "[" << getCurrentTime() << "] Stop " << stopNumber++ << ": " << stopID << " - " << stop->getName() << endl;

            // Update bus location and record in travel history
            bus->setCurrentStop(stopID);
            recordBusStop(busNo, stopID);

            // Check for passengers at this stop
            CircularQueue* queue = getOrCreateQueue(stopID);
            if (!queue->isEmpty()) {
                cout << "   --> " << queue->size() << " passengers waiting at this stop" << endl;
            }

            if (current->next) {
                cout << "   Moving to next stop..." << endl;
                this_thread::sleep_for(chrono::seconds(delaySeconds)); // Sleep for delaySeconds
            }
        }

        current = current->next;
    }

    cout << endl << "Simulation complete! Bus reached final destination." << endl;
}

void TransportSystem::runRealTimeSimulation() {
    cout << endl << "===== Real-Time Bus Simulation =====" << endl;

    int busCount = 0;
    Bus** allBuses = busHash->getAllBuses(busCount);

    if (allBuses == nullptr || busCount == 0) {
        cout << "No buses available for simulation!" << endl;
        return;
    }

    cout << "Available buses:" << endl;
    for (int i = 0; i < busCount; i++) {
        cout << (i + 1) << ". Bus " << allBuses[i]->getBusNo() << " (" << allBuses[i]->getCompany() << ")" << endl;
    }

    cout << endl << "Enter bus number to simulate: ";
    string busNo;
    getline(cin, busNo);

    cout << "Enter delay between stops (seconds): ";
    int delay;
    cin >> delay;
    cin.ignore();

    simulateBusMovement(busNo, delay);

    delete[] allBuses;
}

bool  TransportSystem::readValidSchoolID(Graph* cityGraph, string& outID) {
    while (true) {
        cout << "Enter School ID [ S01, S02 ] or 0 to cancel: ";
        string id;
        getline(cin, id);

        if (id == "0") return false;

        // basic format check: S + 2 digits
        if (id.size() == 3 && id[0] == 'S' &&
            isdigit(id[1]) && isdigit(id[2])) {

            if (cityGraph->hasNode(id)) {
                Node* n = cityGraph->getNode(id);
                if (n && n->getNodeType() == "SCHOOL") {
                    outID = id;
                    return true;
                }
                else {
                    cout << "[!] Node exists but is not a SCHOOL.\n";
                }
            }
            else {
                cout << "[!] School ID not found in graph.\n";
            }
        }
        else {
            cout << "[!] Invalid format. Use S + two digits (e.g., S01).\n";
        }
    }
}

void TransportSystem::displayMenu() {
    cout << endl << "========================================" << endl;
    cout << "     SMART CITY TRANSPORT SYSTEM" << endl;
    cout << "========================================" << endl;
    cout << "Bus & Route Management:" << endl;
    cout << "1.  Register Transport Company" << endl;
    cout << "2.  Register New Bus" << endl;
    cout << "3.  Add Bus Stop" << endl;
    cout << "4.  Connect Two Stops" << endl;
    cout << "5.  Update Bus Location" << endl;
    cout << "6.  Search Bus by Number" << endl;
    cout << "7.  Display All Buses" << endl;
    cout << "8.  Display Network" << endl;
    cout << endl << "Route & Navigation:" << endl;
    cout << "9.  Find Shortest Path" << endl;
    cout << "10. Find Nearest Bus to Location" << endl;
    cout << endl << "Passenger Queue Management (Circular Queue):" << endl;
    cout << "11. Add Passenger to Queue" << endl;
    cout << "12. Board Passengers from Queue" << endl;
    cout << "13. Display Queue at Stop" << endl;
    cout << "14. Simulate Passenger Arrivals" << endl;
    cout << endl << "Travel History (Stack):" << endl;
    cout << "15. Record Bus Stop Visit" << endl;
    cout << "16. Display Bus Travel History" << endl;
    cout << endl << "Airport/Rail Integration:" << endl;
    cout << "17. Add Airport Stop" << endl;
    cout << "18. Add Railway Stop" << endl;
    cout << "19. Find Route to Airport" << endl;
    cout << "20. Display Special Stops" << endl;
    cout << endl << "Simulation:" << endl;
    cout << "21. Simulate Bus Movement" << endl;
    cout << "22. Run Real-Time Simulation" << endl;
    cout << "23. Register School Bus (auto route)" << endl;
    cout << "24. Simulate School Bus (by number)" << endl;

    cout << endl << "0.  Exit" << endl;
    cout << "========================================" << endl;
    cout << "Enter choice: ";
}

void TransportSystem::run() {
    int choice;

    while (true) {
        displayMenu();
        
        if (!(cin >> choice)) {
            cout << endl << "Invalid input! Please enter a number." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();

        if (choice < 0 || choice > 24) {
            cout << endl << "Invalid choice! Please select 0-24." << endl;
            continue;
        }

        if (choice == 0) {
            cout << "Exiting Transport System..." << endl;
            break;
        }

        switch (choice) {
        case 1: {
            string company;
            cout << "Enter company name: ";
            getline(cin, company);

            if (!isValidString(company)) {
                cout << "Company name cannot be empty!" << endl;
                break;
            }

            registerTransportCompany(company);
            break;
        }
        case 2: {
            string busNo, company, currentStop;
            cout << "Enter bus number: ";
            getline(cin, busNo);
            cout << "Enter company name: ";
            getline(cin, company);
            cout << "Enter current stop: ";
            getline(cin, currentStop);
            registerBus(busNo, company, currentStop);
            break;
        }
        case 3: {
            string stopID, name;
            double lat, lon;
            cout << "Enter stop ID: ";
            getline(cin, stopID);
            cout << "Enter stop name: ";
            getline(cin, name);
            cout << "Enter latitude: ";
            cin >> lat;
            cout << "Enter longitude: ";
            cin >> lon;
            cin.ignore();
            addBusStop(stopID, name, lat, lon);
            break;
        }
        case 4: {
            string stop1, stop2;
            cout << "Enter first stop ID: ";
            getline(cin, stop1);
            cout << "Enter second stop ID: ";
            getline(cin, stop2);
            connectStops(stop1, stop2);
            break;
        }
        case 5: {
            string busNo, newStop;
            cout << "Enter bus number: ";
            getline(cin, busNo);
            cout << "Enter new stop: ";
            getline(cin, newStop);
            updateBusLocation(busNo, newStop);
            break;
        }
        case 6: {
            string busNo;
            cout << "Enter bus number: ";
            getline(cin, busNo);
            Bus* bus = getBusByNumber(busNo);
            if (bus) {
                bus->display();
            }
            else {
                cout << "Bus not found!" << endl;
            }
            break;
        }
        case 7: {
            displayAllBuses();
            break;
        }
        case 8: {
            displayNetwork();
            break;
        }
        case 9: {
            string source, dest;
            cout << "Enter source stop: ";
            getline(cin, source);
            cout << "Enter destination stop: ";
            getline(cin, dest);
            findShortestPath(source, dest);
            break;
        }
        case 10: {
            double lat, lon;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cin.ignore();
            searchNearestBus(lat, lon);
            break;
        }
        case 11: {
            string stopID, name, destination;
            cout << "Enter stop ID: ";
            getline(cin, stopID);
            cout << "Enter passenger name: ";
            getline(cin, name);
            cout << "Enter destination: ";
            getline(cin, destination);
            addPassengerToQueue(stopID, name, destination);
            break;
        }
        case 12: {
            string busNo, stopID;
            int count;
            cout << "Enter bus number: ";
            getline(cin, busNo);
            cout << "Enter stop ID: ";
            getline(cin, stopID);
            cout << "Number of passengers to board: ";
            cin >> count;
            cin.ignore();
            boardPassengers(busNo, stopID, count);
            break;
        }
        case 13: {
            string stopID;
            cout << "Enter stop ID: ";
            getline(cin, stopID);
            displayQueueAtStop(stopID);
            break;
        }
        case 14: {
            string stopID;
            cout << "Enter stop ID: ";
            getline(cin, stopID);
            simulatePassengerArrival(stopID);
            break;
        }
        case 15: {
            string busNo, stopID;
            cout << "Enter bus number: ";
            getline(cin, busNo);
            cout << "Enter stop ID: ";
            getline(cin, stopID);
            recordBusStop(busNo, stopID);
            break;
        }
        case 16: {
            string busNo;
            cout << "Enter bus number: ";
            getline(cin, busNo);
            displayBusHistory(busNo);
            break;
        }
        case 17: {
            string stopID, name;
            double lat, lon;
            cout << "Enter airport stop ID: ";
            getline(cin, stopID);
            cout << "Enter airport name: ";
            getline(cin, name);
            cout << "Enter latitude: ";
            cin >> lat;
            cout << "Enter longitude: ";
            cin >> lon;
            cin.ignore();
            addAirportStop(stopID, name, lat, lon);
            break;
        }
        case 18: {
            string stopID, name;
            double lat, lon;
            cout << "Enter railway stop ID: ";
            getline(cin, stopID);
            cout << "Enter railway station name: ";
            getline(cin, name);
            cout << "Enter latitude: ";
            cin >> lat;
            cout << "Enter longitude: ";
            cin >> lon;
            cin.ignore();
            addRailwayStop(stopID, name, lat, lon);
            break;
        }
        case 19: {
            string fromStop;
            cout << "Enter your current stop: ";
            getline(cin, fromStop);
            findRouteToAirport(fromStop);
            break;
        }
        case 20: {
            displaySpecialStops();
            break;
        }
        case 21: {
            string busNo;
            int delay;
            cout << "Enter bus number: ";
            getline(cin, busNo);
            cout << "Enter delay between stops (seconds): ";
            cin >> delay;
            cin.ignore();
            simulateBusMovement(busNo, delay);
            break;
        }
        case 22: {
            runRealTimeSimulation();
            break;
        }
        case 23: {
            string schoolID;
            if (!readValidSchoolID(cityGraph, schoolID)) break;  // user cancelled
            cout << "Valid School ID entered: " << schoolID << endl;
            string busNo, company;
            cout << "Enter new school bus number: ";
            getline(cin, busNo);
            cout << "Enter bus company name: ";
            getline(cin, company);

            registerSchoolBus(schoolID, busNo, company);

            break;
        }

        case 24: {
            string busNo;
            int delay;
            cout << "Enter school bus number: ";
            getline(cin, busNo);
            cout << "Enter delay between stops (seconds): ";
            cin >> delay;
            cin.ignore();
            simulateBusMovement(busNo, delay);
            break;
        }
        default:
            cout << "Invalid choice! Try again." << endl;
        }
    }
}