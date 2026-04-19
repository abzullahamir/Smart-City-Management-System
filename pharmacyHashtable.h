#pragma once
#ifndef PHARMACY_HASHTABLE_H
#define PHARMACY_HASHTABLE_H

#include "medicine.h"
#include <iostream>
#include <string>

using namespace std;

// medicine node for hash chains
struct MedicineNode {
    string key;
    Medicine value;
    MedicineNode* next;

    MedicineNode() : key(""), next(nullptr) {}

    MedicineNode(string k, const Medicine& med)
        : key(k), value(med), next(nullptr) {
    }
};

class PharmacyHashTable {
private:
    int tableSize;
    MedicineNode** nameTable;    // hash by medicine name
    MedicineNode** formulaTable; // hash by formula

    // helper functions
    int stringLen(const string& str) const;
    string toLower(const string& str) const;
    int hash(const string& key) const;

    // internal helpers for search/remove on a specific table
    Medicine* searchInTable(MedicineNode** table, const string& key);
    bool removeFromTable(MedicineNode** table, const string& key);

public:
    PharmacyHashTable(int size = 20);
    ~PharmacyHashTable();

    // insert/update a medicine by name (also indexed by formula)
    void insert(string medicineName, Medicine value);

    // search by medicine name
    Medicine* search(string medicine_name);

    // search by formula
    Medicine* search_by_formula(string formula);

	// both keep name and formula tables in sync
    bool remove(string key);
    bool removeByFormula(string formula);

    // display all medicines
    void display();
};

#endif // PHARMACY_HASHTABLE_H
