//#include "TransportSystem.h"
//#include "CommercialSystem.h"
//#include "educationSystem.h" 
//#include "PopulationSystem.h" // <--- NEW INCLUDE
//#include "C:\Users\LENOVO\source\repos\Dsa-Project -member 2\Dsa-Project -member 2\visualizeSFML.h"
//#include <iostream>
//
//using namespace std;
//
//int main() {
//    cout << "============================================" << endl;
//    cout << "     SMART CITY MANAGEMENT SYSTEM" << endl;
//    cout << "============================================" << endl << endl;
//
//    // 1. Create shared city graph
//    Graph* cityGraph = new Graph();
//    cout << "Creating shared city graph..." << endl << endl;
//
//    // 2. Initialize ALL sectors with the shared graph
//    TransportSystem transport(cityGraph);
//    CommercialSystem commercial(cityGraph);
//    educationSystem education(cityGraph);
//    PopulationSystem population(cityGraph); // <--- NEW INITIALIZATION
//
//    // 3. Load data
//    cout << "=== Loading Transport Data ===" << endl;
//    transport.loadDataFromFiles("stops.csv", "buses.csv");
//
//    cout << endl << "=== Loading Commercial Data ===" << endl;
//    commercial.loadDataFromFiles("malls.csv", "products.csv");
//
//    cout << endl << "=== Loading Education Data ===" << endl;
//    string schoolsFile = "schools.csv";                 // SchoolID,Name,Sector,Rating,Lat,Lon,Subjects...
//
//    string departmentsFile = "department.csv";  // SchoolID,DepartmentName,DepartmentID,ProfessorName,ProfessorID
//
//    string classesFile = "classRoom.csv";        // SchoolID,DepartmentName,ClassName,ClassID,StudentName
//
//    string facultyFile = "faculty.csv";         // SchoolID,DepartmentName,FacultyName,FacultyID,SubjectName
//
//    string studentFile = "students.csv";         // SchoolID,DepartmentName,ClassName,StudentName,StudentID,EnrolledSubjects...
//
//    education.loadEducationData(schoolsFile, departmentsFile, facultyFile, classesFile, studentFile);
//
//    cout << endl << "=== Loading Population Data ===" << endl;
//    population.loadPopulationFromFile("Population.csv"); // <--- NEW LOADING
//
//    cout << endl << "============================================" << endl;
//    cout << "System initialized successfully!" << endl;
//    cout << "City Graph now contains:" << endl;
//    cout << "  - Bus stops" << endl;
//    cout << "  - Malls" << endl;
//    cout << "  - Schools" << endl;
//    cout << "  - Houses" << endl; // <--- Confirmation
//    cout << "Total Nodes: " << cityGraph->getVertexCount() << endl;
//    cout << "============================================" << endl;
//
//    // Main menu
//    int choice;
//    while (true) {
//        cout << endl << "========================================" << endl;
//        cout << "        MAIN MENU" << endl;
//        cout << "========================================" << endl;
//        cout << "1. Transport Sector" << endl;
//        cout << "2. Commercial Sector" << endl;
//        cout << "3. Education Sector" << endl;
//        cout << "4. Population & Housing Sector" << endl; // <--- New Menu Option
//        cout << "5. Display Complete City Graph" << endl;
//        cout << "6. Integrated Query: Route to Nearest Mall" << endl;
//        cout << "7. Integrated Query: Route to Nearest School" << endl;
//        cout << "8. Integrated Query: Route to Specific House" << endl; // <--- New Query
//        cout << "9. Visualize City Graph (SFML)" << endl;
//
//        cout << "0. Exit" << endl;
//        cout << "========================================" << endl;
//        cout << "Enter choice: ";
//
//        if (!(cin >> choice)) {
//            cin.clear();
//            cin.ignore(10000, '\n');
//            continue;
//        }
//        cin.ignore();
//
//        if (choice == 0) {
//            cout << "Thank you for using Smart City System!" << endl;
//            break;
//        }
//
//        switch (choice) {
//        case 1:
//            transport.run();
//            break;
//        case 2:
//            commercial.run();
//            break;
//        case 3:
//            education.run();
//            break;
//        case 4: // <--- Launch Population Menu
//            population.run();
//            break;
//        case 5:
//            cityGraph->displayGraph("ANY");
//            break;
//        case 6: { // Route to Nearest Mall
//            double lat, lon;
//            cout << endl << "=== Integrated Query: Route to Nearest Mall ===" << endl;
//            cout << "Enter your latitude: "; cin >> lat;
//            cout << "Enter your longitude: "; cin >> lon;
//            cin.ignore();
//
//            string nearestStop = transport.findNearestStop(lat, lon);
//            cout << endl << "Nearest bus stop: " << nearestStop << endl;
//
//            string nearestMall = cityGraph->findNearestNode(lat, lon, "MALL");
//            cout << "Nearest mall: " << nearestMall << endl;
//
//            cout << endl << "Finding route..." << endl;
//            cityGraph->dijkstraAlgo(nearestStop, nearestMall, "ANY");
//            break;
//        }
//        case 7: { // Route to Nearest School
//            double lat, lon;
//            cout << endl << "=== Integrated Query: Route to Nearest School ===" << endl;
//            cout << "Enter your latitude: "; cin >> lat;
//            cout << "Enter your longitude: "; cin >> lon;
//            cin.ignore();
//
//            string nearestStop = transport.findNearestStop(lat, lon);
//            cout << endl << "Nearest bus stop: " << nearestStop << endl;
//
//            string nearestSchool = cityGraph->findNearestNode(lat, lon, "SCHOOL");
//            cout << "Nearest school ID: " << nearestSchool << endl;
//
//            if (nearestSchool != "") {
//                cout << endl << "Finding route..." << endl;
//                cityGraph->dijkstraAlgo(nearestStop, nearestSchool, "ANY");
//            }
//            else {
//                cout << "No schools found in the graph." << endl;
//            }
//            break;
//        }
//        case 8: { // <--- New Query: Route to House
//            // This demonstrates finding a specific house on the map
//            // Format for House ID depends on your PopulationSystem logic: "H-Sector-HouseNum"
//            string sector;
//            int houseNum;
//            string startNode;
//
//            cout << endl << "=== Integrated Query: Route to Specific House ===" << endl;
//            cout << "Enter your current location (Node ID, e.g., S01): ";
//            getline(cin, startNode);
//
//            cout << "Enter Target Sector (e.g., G-10): ";
//            getline(cin, sector);
//            cout << "Enter Target House Number: ";
//            cin >> houseNum;
//
//            // Construct Graph ID based on your PopulationSystem logic
//            string houseID = "H-" + sector + "-" + to_string(houseNum);
//
//            if (cityGraph->hasNode(houseID)) {
//                cout << "House found on map (" << houseID << ")." << endl;
//                cout << "Finding route..." << endl;
//                cityGraph->dijkstraAlgo(startNode, houseID, "ANY");
//            }
//            else {
//                cout << "[!] House not found on the map layer." << endl;
//            }
//            break;
//        }
//        case 9:
//           visualizeCity(cityGraph);
//            break;
//
//        default:
//            cout << "Invalid choice! Try again." << endl;
//        }
//    }
//
//    delete cityGraph;
//    return 0;
//}