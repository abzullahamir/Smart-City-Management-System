#pragma once
#include "Graph.h"
#include "familyTree.h"
#include "cnincHashTable.h" 
#include "populationStats.h"
#include "populationTree.h"
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;



class PopulationSystem {
    const string cityName = "Islamabad";
    int maleCount;
    int femaleCount;

    // Core Data Structures
    sectorNode* sectorHead;
    CNICHashTable* cnicTable;

    // Graph Integration
    Graph* cityGraph;
    bool ownsGraph;

    // --- PRIVATE HELPERS ---

    double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        double diffLat = (lat2 - lat1) * PI / 180.0;
        double diffLon = (lon2 - lon1) * PI / 180.0;
        lat1 = lat1 * PI / 180.0;
        lat2 = lat2 * PI / 180.0;
        double a = sin(diffLat / 2) * sin(diffLat / 2) + sin(diffLon / 2) * sin(diffLon / 2) * cos(lat1) * cos(lat2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        return EARTH_RADIUS * c;
    }

    string getField(string line, int index) {
        int current = 0;
        size_t start = 0;
        size_t end = line.find(',');
        while (end != string::npos) {
            if (current == index) return line.substr(start, end - start);
            start = end + 1;
            end = line.find(',', start);
            current++;
        }
        if (current == index) return line.substr(start);
        return "";
    }

    bool checkHouseVulnerability(personNode* p) {
        if (!p) return false;
        if (p->age >= 60 || p->age <= 5) return true;

        // Recursive checks
        if (p->spouse && checkHouseVulnerability(p->spouse)) return true;
        if (p->childHead && checkHouseVulnerability(p->childHead)) return true;
        if (p->next && checkHouseVulnerability(p->next)) return true;
        return false;
    }

public:
    // --- CONSTRUCTOR & DESTRUCTOR ---
    PopulationSystem(Graph* sharedGraph = nullptr) {
        sectorHead = nullptr;
        cnicTable = new CNICHashTable();
        maleCount = 0;
        femaleCount = 0;

        if (sharedGraph) {
            this->cityGraph = sharedGraph;
            ownsGraph = false;
        }
        else {
            this->cityGraph = new Graph();
            ownsGraph = true;
        }
    }

    ~PopulationSystem() {
        if (cnicTable) delete cnicTable;
        if (sectorHead) {
            sectorNode* temp = sectorHead;
            while (temp != nullptr) {
                sectorNode* toDelete = temp;
                temp = temp->next;
                delete toDelete; 
            }
        }
        if (ownsGraph) delete cityGraph;
    }

    // --- MAP BUILDING ---
    void addSector(string sectorName) {
        sectorNode* newSector = new sectorNode(sectorName);
        if (sectorHead == nullptr) sectorHead = newSector;
        else {
            sectorNode* temp = sectorHead;
            while (temp->next != nullptr) temp = temp->next;
            temp->next = newSector;
        }
    }

    sectorNode* searchSectorNode(string sectorName) {
        sectorNode* temp = sectorHead;
        while (temp != nullptr) {
            if (temp->sectorName == sectorName) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    sectorNode* getOrCreateSector(string name) {
        sectorNode* temp = searchSectorNode(name);
        if (temp != nullptr) return temp;
        addSector(name);
        return searchSectorNode(name);
    }

    // --- FILE LOADING ---
    void loadPopulationFromFile(string fileName) {
        ifstream file(fileName);
        if (!file.is_open()) {
            cout << "Error: File " << fileName << " not found!" << endl;
            return;
        }

        string line;
        getline(file, line); // Skip Header

        while (getline(file, line)) {
            if (line.empty()) continue;

            string cnic = getField(line, 0);
            string name = getField(line, 1);
            string ageStr = getField(line, 2);
            string sector = getField(line, 3);
            string streetStr = getField(line, 4);
            string houseStr = getField(line, 5);
            string occupation = getField(line, 6);
            string genderStr = getField(line, 7);
            string relation = getField(line, 8);
            string parentCNIC = getField(line, 9);
            string latStr = getField(line, 10);
            string lonStr = getField(line, 11);

            if (!relation.empty() && relation.back() == '\r') relation.pop_back();
            if (!parentCNIC.empty() && parentCNIC.back() == '\r') parentCNIC.pop_back();

            int age = 0, houseNum = 0, streetNum = 0;
            double lat = 0.0, lon = 0.0;

            try { if (!ageStr.empty()) age = stoi(ageStr); }
            catch (...) {}
            try { if (!houseStr.empty()) houseNum = stoi(houseStr); }
            catch (...) {}
            try { if (!streetStr.empty()) streetNum = stoi(streetStr); }
            catch (...) {}
            try { if (!latStr.empty()) lat = stod(latStr); }
            catch (...) {}
            try { if (!lonStr.empty()) lon = stod(lonStr); }
            catch (...) {}

            char gender = (genderStr.empty()) ? 'M' : genderStr[0];

            //  Build Map Layer
            sectorNode* secNode = getOrCreateSector(sector);
            streetNode* stNode = secNode->getOrCreateStreet(streetNum);

            // Handle House & Coordinates
            houseNode* house = stNode->searchHouseNode(houseNum);
            if (house == nullptr) {
                stNode->addHouse(houseNum);
                house = stNode->searchHouseNode(houseNum);
                house->latitude = lat;
                house->longitude = lon;
            }

            //  Create Person
            personNode* newPerson = new personNode(name, cnic, occupation, age);
            newPerson->gender = gender;
            newPerson->relation = relation;
            (newPerson->gender == 'M') ? maleCount++ : femaleCount++;

            // Graph Integration 
            if (cityGraph != nullptr) {
                string houseID = "H-" + sector + "-" + to_string(houseNum);
                if (!cityGraph->hasNode(houseID)) {
                    Node houseGraphNode(houseID, "House " + to_string(houseNum) + " " + sector, "HOUSE", lat, lon);
                    cityGraph->addNode(houseGraphNode);
                    cityGraph->connectToNearbyNodes(houseID, 3 , "BUS_STOP");
                }
            }

            // 4. Family Tree Link Logic
            bool addedToTree = false;
            if (!parentCNIC.empty()) {
                personNode* parent = cnicTable->search(parentCNIC);
                if (parent) {
                    parent->addChild(newPerson);
                    addedToTree = true;
                }
            }

            if (!addedToTree) {
                if (house->familyHead == nullptr) {
                    house->setFamilyHead(newPerson);
                    if (newPerson->relation.empty()) newPerson->relation = "Head";
                }
                else {
                    // Smart Insertion / Swap Logic
                    if (relation == "Head") {
                        personNode* oldHead = house->familyHead;
                        house->setFamilyHead(newPerson);
                        newPerson->addChild(oldHead);
                    }
                    else if (relation == "Spouse") {
                        house->familyHead->addSpouse(newPerson);
                    }
                    else {
                        house->familyHead->addChild(newPerson);
                    }
                }
            }
            cnicTable->insert(cnic, newPerson);
        }
        file.close();
        cout << "Population loaded successfully from " << fileName << endl;
    }

    // --- REPORTS & DISPLAY ---
    void displayCity() {
        cout << "\n==============================================" << endl;
        cout << "          CITY: " << cityName << endl;
        cout << "==============================================" << endl;
        sectorNode* temp = sectorHead;
        while (temp != nullptr) {
            temp->displaySector();
            temp = temp->next;
        }
        cout << "==============================================\n" << endl;
    }

    void generateSectorReport(string sectorName) {
        sectorNode* s = searchSectorNode(sectorName);
        if (!s) { cout << "Sector " << sectorName << " not found!" << endl; return; }

        PopulationStats* stats = new PopulationStats;
        stats->collectFamilyDataFromSector(s);
        stats->printReport("SECTOR REPORT: " + sectorName);
        delete stats;
    }

    void generateCityReport() {
        PopulationStats* stats = new PopulationStats;
        sectorNode* temp = sectorHead;
        while (temp) {
            stats->collectFamilyDataFromSector(temp);
            temp = temp->next;
        }
        stats->printReport("CITY REPORT: ISLAMABAD (ALL SECTORS)");
        delete stats;
    }

    void generateHeatMap() {
        cout << "\n==============================================" << endl;
        cout << "       SMART CITY POPULATION HEATMAP" << endl;
        cout << "==============================================" << endl;
        cout << "Density Scale: [#] = 1 person \n" << endl;

        sectorNode* sec = sectorHead;
        if (!sec) { cout << "No sectors data available." << endl; return; }

        while (sec) {
            PopulationStats* tempStats = new PopulationStats();
            tempStats->collectFamilyDataFromSector(sec);
            int pop = tempStats->totalPopulation;
            delete tempStats;

            cout << "SECTOR " << sec->sectorName << ": ";
            if (pop == 0) cout << "(Empty)" << endl;
            else {
                int bars = pop ;
                if (bars == 0 && pop > 0) bars = 1;
                for (int i = 0; i < bars; i++) 
                    cout << "#";
                cout << " (" << pop << ")" << endl;
            }
            cout << "----------------------------------------------" << endl;
            sec = sec->next;
        }
        cout << endl;
    }

    // --- SMART CITY FEATURES ---
    void searchCitizen(string cnic) {
        cnicTable->searchAndDisplay(cnic);
    }

    void findVulnerableHouseholds(string sectorName) {
        sectorNode* sec = searchSectorNode(sectorName);
        if (!sec) { cout << "Sector not found." << endl; return; }

        cout << "\n=== VULNERABLE HOUSEHOLDS ALERT (Sector " << sectorName << ") ===" << endl;
        bool foundAny = false;
        streetNode* street = sec->streetHead;
        while (street) {
            houseNode* house = street->houseHead;
            while (house) {
                if (checkHouseVulnerability(house->familyHead)) {
                    cout << " [ALERT] House " << house->houseNumber << ", Street " << street->streetNumber
                        << " -> Contains Vulnerable Residents" << endl;
                    foundAny = true;
                }
                house = house->next;
            }
            street = street->next;
        }
        if (!foundAny) cout << "No vulnerable households found in this sector." << endl;
    }

    void findNearestFacility(string sectorName, int streetNumber, int houseNumber, string facilityType) {
        sectorNode* sec = searchSectorNode(sectorName);
        if (!sec) { cout << "Sector not found." << endl; return; }

        streetNode* street = sec->searchStreetNode(streetNumber);
        if (!street) { cout << "Street not found." << endl; return; }

        houseNode* house = street->searchHouseNode(houseNumber);
        if (!house) { cout << "House not found." << endl; return; }

        cout << "\n=== NEAREST " << facilityType << " SEARCH ===" << endl;
        string nearestID = cityGraph->findNearestNode(house->latitude, house->longitude, facilityType);

        if (nearestID.empty()) {
            cout << "[!] No " << facilityType << " found in the network." << endl;
        }
        else {
            Node* targetNode = cityGraph->getNode(nearestID);
            double dist = 0.0;
            if (targetNode) {
                dist = calculateDistance(house->latitude, house->longitude, targetNode->getLatitude(), targetNode->getLongitude());
                cout << "Nearest: " << targetNode->getName() << " (ID: " << nearestID << ")" << endl;
            }
            else {
                cout << "Nearest ID: " << nearestID << endl;
            }
            cout << "Distance: " << dist << " km" << endl;
        }
    }

    void planCommuteInteractive() {
        cout << "\n==============================================" << endl;
        cout << "       COMMUTE PLANNER (Home -> Destination)" << endl;
        cout << "==============================================" << endl;

        displayAvailableSectors();
        string sector;
        cout << "Enter Sector Name : "; cin >> sector;

        sectorNode* secNode = searchSectorNode(sector);
        if (!secNode) { cout << "[!] Invalid Sector Name." << endl; return; }

        displayAvailableStreets(sector);
        int streetNum;
        cout << "Enter Street Number: "; cin >> streetNum;

        streetNode* stNode = secNode->searchStreetNode(streetNum);
        if (!stNode) { cout << "[!] Street not found." << endl; return; }

        displayAvailableHouses(sector, streetNum);
        int houseNum;
        cout << "Enter House Number: "; cin >> houseNum;

        houseNode* house = stNode->searchHouseNode(houseNum);
        if (!house) { cout << "[!] House not found." << endl; return; }

        string destID;
        cout << "Enter Destination ID (e.g., S01, Stop1): "; cin >> destID;

        string startNodeID = "H-" + sector + "-" + to_string(houseNum);
        cout << "\n[System] Calculating shortest path via Dijkstra..." << endl;

        if (cityGraph->hasNode(startNodeID)) {
            cityGraph->dijkstraAlgo(startNodeID, destID, "ANY");
        }
        else {
            cout << "[Error] House found in database but not on map layer." << endl;
        }
    }

    // --- MENU HELPERS ---
    void displayAvailableSectors() {
        cout << "\n--- Available Sectors ---" << endl;
        sectorNode* temp = sectorHead;
        int count = 0;
        while (temp) {
            cout << " [" << temp->sectorName << "] ";
            temp = temp->next;
            count++;
            if (count % 5 == 0) cout << endl;
        }
        cout << endl;
    }

    void displayAvailableStreets(string sectorName) {
        sectorNode* sec = searchSectorNode(sectorName);
        if (!sec) return;
        cout << "--- Streets in " << sectorName << " ---" << endl;
        streetNode* st = sec->streetHead;
        while (st) {
            cout << "[St " << st->streetNumber << "] ";
            st = st->next;
        }
        cout << endl;
    }

    void displayAvailableHouses(string sectorName, int streetNumber) {
		sectorNode* sec = searchSectorNode(sectorName);
		if (!sec) return;
		streetNode* st = sec->searchStreetNode(streetNumber);
		if (!st) return;
		cout << "--- Houses in " << sectorName << ", Street " << streetNumber << " ---" << endl;
		houseNode* house = st->houseHead;
		while (house) {
			cout << "[House " << house->houseNumber << "] ";
			house = house->next;
		}
		cout << endl;
	}

    void displayFamilyByCNIC(const string& cnic) {
        personNode* p = cnicTable->search(cnic);
        cout << "\n   ===================================================" << endl;
        cout << "    Family Tree Lookup for CNIC: " << cnic << endl;
        cout << "   ===================================================" << endl;

        if (!p) {
            cout << "    [!] No citizen found with this CNIC.\n";
            cout << "   ===================================================\n" << endl;
            return;
        }

        cout << "    -> Name: " << p->name << endl;
        cout << "    -> Age: " << p->age << endl;
        cout << "    -> Occupation: " << p->occupation << endl;
        cout << "    -> Relation: " << p->relation << endl;
        cout << "\n    [Family Tree]\n";
        p->displayFamilyTree();   
        cout << "   ===================================================\n" << endl;
    }


    // --- MAIN MENU & RUN ---
    void displayMenu() {
        cout << endl << "========================================" << endl;
        cout << "      POPULATION & HOUSING MANAGEMENT" << endl;
        cout << "========================================" << endl;
        cout << "--- DISPLAY & SEARCH ---" << endl;
        cout << "1.  Display Full City Hierarchy" << endl;
        cout << "2.  Search Citizen (CNIC)" << endl;
        cout << "3.  Show Family Tree by CNIC\n";

        cout << "4.  View Population Heatmap (Visual)" << endl;

        cout << "\n--- ANALYTICS & REPORTS ---" << endl;
        cout << "5.  Generate City-Wide Report" << endl;
        cout << "6.  Generate Sector-Specific Report" << endl;
        cout << "7.  [EMERGENCY] Find Vulnerable Households" << endl;

        cout << "\n--- SMART CITY SERVICES ---" << endl;
        cout << "8.  Find Nearest Facility (from House)" << endl;
        cout << "9.  Plan Commute (Route Finding)" << endl;

        cout << "0.  Exit" << endl;
        cout << "========================================" << endl;
        cout << "Enter choice: ";
    }

    void run() {
        int choice;
        while (true) {
            displayMenu();
            if (!(cin >> choice)) { cin.clear(); cin.ignore(10000, '\n'); continue; }
            cin.ignore();

            if (choice == 0) { cout << "Exiting Population Module..." << endl; break; }

            switch (choice) {
            case 1: displayCity(); break;
            case 2: {
                string cnic;
                cout << "Enter CNIC: ";
                cin >> cnic;
                searchCitizen(cnic);        // your existing simple search + info
                break;
            }
            case 3: {
                string cnic;
                cout << "Enter CNIC for family tree: ";
                cin >> cnic;
                displayFamilyByCNIC(cnic);  // new detailed family tree view
                break;
            }
                  
    

            case 4: generateHeatMap(); break;
            case 5: generateCityReport(); break;
            case 6: {
                string s;
                displayAvailableSectors();
                cout << "Enter Sector: "; cin >> s;
                generateSectorReport(s);
                break;
            }
            case 7: {
                string s;
                displayAvailableSectors();
                cout << "Enter Sector to Scan: "; cin >> s;
                findVulnerableHouseholds(s);
                break;
            }
            case 8: {
                displayAvailableSectors();
                string sector;
                cout << "Enter Sector Name (e.g., G-10): "; cin >> sector;
                sectorNode* secNode = searchSectorNode(sector);
                if (!secNode) { cout << "[!] Invalid Sector Name." << endl; return; }
               

                displayAvailableStreets(sector);
                int streetNum;
                cout << "Enter Street Number: "; cin >> streetNum;
                streetNode* stNode = secNode->searchStreetNode(streetNum);
                if (!stNode) { cout << "[!] Street not found." << endl; return; }

                displayAvailableHouses(sector, streetNum);
                int houseNum;
                cout << "Enter House Number: "; cin >> houseNum;
                houseNode* house = stNode->searchHouseNode(houseNum);
                if (!house) { cout << "[!] House not found." << endl; return; }

                string type;
                cout << "Facility Type (BUS_STOP / MALL / SCHOOL): "; cin >> type; 
                findNearestFacility(sector, streetNum, houseNum, type);
                break;
            }
            case 9: planCommuteInteractive(); break;
            default: cout << "Invalid choice.\n";
            }
        }
    }
};