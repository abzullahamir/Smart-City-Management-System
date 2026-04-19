#include "TransportSystem.h"
#include "CommercialSystem.h"
#include "educationSystem.h"
#include "PopulationSystem.h"
#include "MedicalSystem.h"
#include "PublicFacilitiesSystem.h"
#include "hospitals.h"
#include "visualizeSFML.h"       // just declare: void visualizeCity(Graph*);
#include <iostream>
using namespace std;

int main() {
    cout << "============================================" << endl;
    cout << "     SMART CITY MANAGEMENT SYSTEM" << endl;
    cout << "============================================" << endl << endl;

    // 1. Shared city graph
    Graph* cityGraph = new Graph();
    cout << "Creating shared city graph..." << endl << endl;

    // 2. Initialize all sectors with shared graph
    TransportSystem       transport(cityGraph);
    CommercialSystem      commercial(cityGraph);
    educationSystem       education(cityGraph);
    PopulationSystem      population(cityGraph);
    MedicalSystem         medical(cityGraph);
    PublicFacilitiesSystem publicFacilities(cityGraph);
    SFMLGraphVisualizer viz(cityGraph);

    // 3. Load data
    cout << "=== Loading Transport Data ===" << endl;
    transport.loadDataFromFiles("stops.csv", "buses.csv");

    cout << endl << "=== Loading Commercial Data ===" << endl;
    commercial.loadDataFromFiles("malls.csv", "products.csv");

    cout << endl << "=== Loading Education Data ===" << endl;
    string schoolsFile = "schools.csv";
    string departmentsFile = "department.csv";
    string classesFile = "classRoom.csv";
    string facultyFile = "faculty.csv";
    string studentFile = "students.csv";
    education.loadEducationData(schoolsFile, departmentsFile,
        facultyFile, classesFile, studentFile);

    cout << endl << "=== Loading Population Data ===" << endl;
    population.loadPopulationFromFile("population02.csv");

    cout << endl << "=== Loading Medical Data ===" << endl;
    medical.loadDataFromFiles("hospitals.csv", "pharmacies.csv");

    cout << endl << "=== Loading Public Facilities ===" << endl;
    publicFacilities.loadPublicFacilities("public_facilities.csv");

    cout << endl << "============================================" << endl;
    cout << "System initialized successfully!" << endl;
    cout << "City Graph now contains:" << endl;
    cout << "  - Bus stops" << endl;
    cout << "  - Malls" << endl;
    cout << "  - Schools" << endl;
    cout << "  - Houses" << endl;
    cout << "  - Hospitals & Pharmacies" << endl;
    cout << "  - Mosques, Parks & Water Coolers" << endl;
    cout << "Total Nodes: " << cityGraph->getVertexCount() << endl;
    cout << "============================================" << endl;

    cityGraph->exportEdgesCSV();
    cityGraph->exportNodesCSV();

    // Main menu
    int choice;
    while (true) {
        cout << endl << "========================================" << endl;
        cout << "                MAIN MENU" << endl;
        cout << "========================================" << endl;
        cout << "1.  Transport Sector" << endl;
        cout << "2.  Commercial Sector" << endl;
        cout << "3.  Education Sector" << endl;
        cout << "4.  Population & Housing Sector" << endl;
        cout << "5.  Medical Sector" << endl;
        cout << "6.  Public Facilities Sector" << endl;
        cout << "7.  Display Complete City Graph" << endl;
        cout << "8.  Integrated: Route to Nearest Mall" << endl;
        cout << "9.  Integrated: Route to Nearest School" << endl;
        cout << "10. Integrated: Route to Specific House" << endl;
        cout << "11. Integrated: Route to Nearest Hospital" << endl;
        cout << "12. Integrated: Route to Nearest Pharmacy" << endl;
        cout << "13. Integrated: Route to Nearest Mosque" << endl;
        cout << "14. Integrated: Route to Nearest Park" << endl;
        cout << "15. Integrated: Route to Nearest Water Cooler" << endl;
        cout << "16. Emergency Transport Routing" << endl;
        cout << "17. Visualize City Graph (SFML)" << endl;
        cout << "0.  Exit" << endl;
        cout << "========================================" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();

        if (choice == 0) {
            cout << "Thank you for using Smart City System!" << endl;
            break;
        }

        switch (choice) {
        case 1: transport.run(); break;
        case 2: commercial.run(); break;
        case 3: education.run(); break;
        case 4: population.run(); break;
        case 5: medical.run(); break;
        case 6: publicFacilities.run(); break;

        case 7:
            cityGraph->displayGraph("ANY");
            break;

        case 8: { // Nearest Mall
            double lat, lon;
            cout << endl << "=== Integrated: Route to Nearest Mall ===" << endl;
            cout << "Enter your latitude: ";  cin >> lat;
            cout << "Enter your longitude: "; cin >> lon;
            cin.ignore();

            string nearestStop = transport.findNearestStop(lat, lon);
            cout << endl << "Nearest bus stop: " << nearestStop << endl;

            string nearestMall = cityGraph->findNearestNode(lat, lon, "MALL");
            cout << "Nearest mall: " << nearestMall << endl;

            cout << endl << "Finding route..." << endl;
            cityGraph->dijkstraAlgo(nearestStop, nearestMall, "ANY");
            break;
        }

        case 9: { // Nearest School
            double lat, lon;
            cout << endl << "=== Integrated: Route to Nearest School ===" << endl;
            cout << "Enter your latitude: ";  cin >> lat;
            cout << "Enter your longitude: "; cin >> lon;
            cin.ignore();

            string nearestStop = transport.findNearestStop(lat, lon);
            cout << endl << "Nearest bus stop: " << nearestStop << endl;

            string nearestSchool = cityGraph->findNearestNode(lat, lon, "SCHOOL");
            cout << "Nearest school: " << nearestSchool << endl;

            if (!nearestSchool.empty()) {
                cout << endl << "Finding route..." << endl;
                cityGraph->dijkstraAlgo(nearestStop, nearestSchool, "ANY");
            }
            else {
                cout << "No schools found in the graph." << endl;
            }
            break;
        }

        case 10: { // Route to specific house
            string sector, startNode;
            int houseNum;

            cout << endl << "=== Integrated: Route to Specific House ===" << endl;
            cout << "Enter your current location (Node ID, e.g., S01): ";
            getline(cin, startNode);

            cout << "Enter Target Sector (e.g., G-10): ";
            getline(cin, sector);
            cout << "Enter Target House Number: ";
            cin >> houseNum;
            cin.ignore();

            string houseID = "H-" + sector + "-" + to_string(houseNum);

            if (cityGraph->hasNode(houseID)) {
                cout << "House found on map (" << houseID << ")." << endl;
                cout << "Finding route..." << endl;
                cityGraph->dijkstraAlgo(startNode, houseID, "ANY");
            }
            else {
                cout << "[!] House not found on the map layer." << endl;
            }
            break;
        }

        case 11: { // Nearest Hospital
            double lat, lon;
            cout << endl << "=== Integrated: Route to Nearest Hospital ===" << endl;
            cout << "Enter your latitude: ";  cin >> lat;
            cout << "Enter your longitude: "; cin >> lon;
            cin.ignore();

            string nearestStop = transport.findNearestStop(lat, lon);
            cout << endl << "Nearest bus stop: " << nearestStop << endl;

            string nearestHospital = cityGraph->findNearestNode(lat, lon, "HOSPITAL");
            cout << "Nearest hospital: " << nearestHospital << endl;

            cout << endl << "Finding route..." << endl;
            cityGraph->dijkstraAlgo(nearestStop, nearestHospital, "ANY");
            break;
        }

        case 12: { // Nearest Pharmacy
            double lat, lon;
            cout << endl << "=== Integrated: Route to Nearest Pharmacy ===" << endl;
            cout << "Enter your latitude: ";  cin >> lat;
            cout << "Enter your longitude: "; cin >> lon;
            cin.ignore();

            string nearestStop = transport.findNearestStop(lat, lon);
            cout << endl << "Nearest bus stop: " << nearestStop << endl;

            string nearestPharmacy = cityGraph->findNearestNode(lat, lon, "PHARMACY");
            cout << "Nearest pharmacy: " << nearestPharmacy << endl;

            cout << endl << "Finding route..." << endl;
            cityGraph->dijkstraAlgo(nearestStop, nearestPharmacy, "ANY");
            break;
        }

        case 13: { // Nearest Mosque
            double lat, lon;
            cout << endl << "=== Integrated: Route to Nearest Mosque ===" << endl;
            cout << "Enter your latitude: ";  cin >> lat;
            cout << "Enter your longitude: "; cin >> lon;
            cin.ignore();

            string nearestStop = transport.findNearestStop(lat, lon);
            cout << endl << "Nearest bus stop: " << nearestStop << endl;

            string nearestMosque = cityGraph->findNearestNode(lat, lon, "MOSQUE");
            cout << "Nearest mosque: " << nearestMosque << endl;

            cout << endl << "Finding route..." << endl;
            cityGraph->dijkstraAlgo(nearestStop, nearestMosque, "ANY");
            break;
        }

        case 14: { // Nearest Park
            double lat, lon;
            cout << endl << "=== Integrated: Route to Nearest Park ===" << endl;
            cout << "Enter your latitude: ";  cin >> lat;
            cout << "Enter your longitude: "; cin >> lon;
            cin.ignore();

            string nearestStop = transport.findNearestStop(lat, lon);
            cout << endl << "Nearest bus stop: " << nearestStop << endl;

            string nearestPark = cityGraph->findNearestNode(lat, lon, "PARK");
            cout << "Nearest park: " << nearestPark << endl;

            cout << endl << "Finding route..." << endl;
            cityGraph->dijkstraAlgo(nearestStop, nearestPark, "ANY");
            break;
        }

        case 15: { // Nearest Water Cooler
            double lat, lon;
            cout << endl << "=== Integrated: Route to Nearest Water Cooler ===" << endl;
            cout << "Enter your latitude: ";  cin >> lat;
            cout << "Enter your longitude: "; cin >> lon;
            cin.ignore();

            string nearestStop = transport.findNearestStop(lat, lon);
            cout << endl << "Nearest bus stop: " << nearestStop << endl;

            string nearestCooler = cityGraph->findNearestNode(lat, lon, "WATER_COOLER");
            cout << "Nearest water cooler: " << nearestCooler << endl;

            cout << endl << "Finding route..." << endl;
            cityGraph->dijkstraAlgo(nearestStop, nearestCooler, "ANY");
            break;
        }

        case 16: { // Emergency Transport Routing
            double lat, lon;
            cout << endl;
            cout << "EMERGENCY TRANSPORT ROUTING" << endl;
            cout << "---------------------------" << endl;

            cout << "Enter patient's latitude: ";
            cin >> lat;
            cout << "Enter patient's longitude: ";
            cin >> lon;
            cin.ignore();

            string spec;
            cout << "Enter required specialization (or leave empty for any): ";
            getline(cin, spec);

            // 1) Choose best emergency hospital
            Hospital* targetHospital = nullptr;

            if (!spec.empty()) {
                // Prefer hospital that matches specialization AND has beds
                targetHospital = medical.findOptimalHospital(lat, lon, spec, 1);
            }

            if (targetHospital == nullptr) {
                // Fallback: nearest hospital with at least 1 emergency bed
                targetHospital = medical.findNearestHospitalWithBeds(lat, lon, 1);
            }

            if (targetHospital == nullptr) {
                // Last fallback: nearest hospital ignoring beds
                targetHospital = medical.findNearestHospital(lat, lon);
            }

            if (targetHospital == nullptr) {
                cout << endl << "No suitable hospital found for emergency routing." << endl;
                break;
            }

            cout << endl;
            cout << "Selected Emergency Hospital: " << targetHospital->name
                << " (" << targetHospital->hospitalID << ")" << endl;
            cout << "Sector: " << targetHospital->sector << endl;
            cout << "Available Emergency Beds: " << targetHospital->emergencyBeds << endl;

            // 2) Map patient location to nearest bus stop
            string nearestStop = transport.findNearestStop(lat, lon);
            cout << endl << "Nearest bus stop to patient: " << nearestStop << endl;

            // 3) Compute and display shortest route via shared city graph
            cout << endl << "Finding shortest emergency route..." << endl;
            medical.findRouteToHospital(nearestStop, targetHospital->hospitalID);

            // 4)  reserve an emergency bed
            char reserveChoice;
            cout << endl << "Reserve an emergency bed at this hospital? (y/n): ";
            cin >> reserveChoice;
            cin.ignore();

            if (reserveChoice == 'y' || reserveChoice == 'Y') {

                medical.reserveEmergencyBed(targetHospital->hospitalID);
            }

            break;
        }

        case 17:
            
            viz.visualize("ANY");
            break;

        default:
            cout << "Invalid choice! Try again." << endl;
        }
    }

    delete cityGraph;
    return 0;
}