#pragma once
#ifndef PHARMACY_CLASS_H
#define PHARMACY_CLASS_H

#include "pharmacyHashtable.h"
#include <iostream>
#include <string>

using namespace std;

class pharmacy {
private:
    string pharmacyID;
    string pharmacyName;
    string sector;
    double latitude;
    double longitude;
    PharmacyHashTable* medicineList;

public:
    // Constructor with coordinates and initial medicine
    pharmacy(string id, string name, string sec,
        double lat, double lon,
        const Medicine& initialMed,
        int tableSize = 20)
        : pharmacyID(id),
        pharmacyName(name),
        sector(sec),
        latitude(lat),
        longitude(lon) {
        medicineList = new PharmacyHashTable(tableSize);
        medicineList->insert(initialMed.medicineName, initialMed);
        cout << "Pharmacy " << pharmacyName << " created in " << sector << "\n";
    }

    // Constructor without initial medicine
    pharmacy(string id, string name, string sec,
        double lat, double lon,
        int tableSize = 20)
        : pharmacyID(id),
        pharmacyName(name),
        sector(sec),
        latitude(lat),
        longitude(lon) {
        medicineList = new PharmacyHashTable(tableSize);
        cout << "Pharmacy " << pharmacyName << " created in " << sector << "\n";
    }

    ~pharmacy() {
        delete medicineList;
    }

    // Add medicine to this pharmacy
    void addMedicine(const Medicine& med) {
        medicineList->insert(med.medicineName, med);
    }

    Medicine* searchByName(const string& name) {
        return medicineList->search(name);
    }

    Medicine* searchByFormula(const string& formula) {
        return medicineList->search_by_formula(formula);
    }

    // Display all medicines in this pharmacy
    void displayMedicines() {
        medicineList->display();
    }

    string getID() const { return pharmacyID; }
    string getName() const { return pharmacyName; }
    string getSector() const { return sector; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }

    void displayPharmacyDetails() {
        cout << "\n=== " << pharmacyName << " ===\n";
        cout << "ID: " << pharmacyID << "\n";
        cout << "Sector: " << sector << "\n";
        cout << "Location: (" << latitude << ", " << longitude << ")\n";
        cout << "\nAvailable Medicines:\n";
        medicineList->display();
    }

    // keeps both name/formula indexes in sync
    bool removeMedicine(const string& medicineName) {
        if (medicineName.empty()) {
            cout << "Medicine name cannot be empty.\n";
            return false;
        }

        bool removed = medicineList->remove(medicineName);
        if (!removed) {
            cout << "Medicine " << medicineName
                << " not found in this pharmacy.\n";
        }
        return removed;
    }
};

#endif // PHARMACY_CLASS_H
