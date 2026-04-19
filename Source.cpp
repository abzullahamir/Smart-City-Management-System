#include <iostream>
#include <string>
#include "PopulationSystem.h"

using namespace std;

int main() {
    // 1. Initialize
    cout << "--- Smart City Population System Starting ---" << endl;
    PopulationSystem popSys;

    // 2. Load Data
    // Ensure "Population.csv" is in the same folder!
    popSys.loadPopulationFromFile("Population.csv");

    // 3. Interactive Menu
    int choice = -1;
    while (choice != 0) {
        cout << "\n---------------- MENU ----------------" << endl;
        cout << "1. Display City Map (Sectors/Streets/Families)" << endl;
        cout << "2. Search Citizen by CNIC" << endl;
        cout << "3. Generate City-Wide Report" << endl;
        cout << "4. Generate Sector-Specific Report" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            popSys.displayCity();
        }
        else if (choice == 2) {
            string cnic;
            cout << "Enter CNIC: ";
            cin >> cnic;
            popSys.searchCitizen(cnic);
        }
        else if (choice == 3) {
            popSys.generateCityReport();
        }
        else if (choice == 4) {
            string sec;
            cout << "Enter Sector Name (e.g., G-10): ";
            cin >> sec;
            popSys.generateSectorReport(sec);
        }
        else if (choice == 0) {
            cout << "Exiting..." << endl;
        }
        else {
            cout << "Invalid choice!" << endl;
        }
    }

    return 0;
}