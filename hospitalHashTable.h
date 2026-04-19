#pragma once
#ifndef HOSPITAL_HASHTABLE_H
#define HOSPITAL_HASHTABLE_H

#include <iostream>
#include <string>
#include "hospitals.h"

using namespace std;

struct HospitalHashNode {
    string key;
    Hospital* value;
    HospitalHashNode* next;

    HospitalHashNode() : value(nullptr), next(nullptr) {}
    HospitalHashNode(string k, Hospital* h) : key(k), value(h), next(nullptr) {}
};

class HospitalHashTable {
private:
    int tableSize;
    HospitalHashNode** table;

    bool stringsEqualCaseInsensitive(const string& s1, const string& s2) const;
    string toLowerCase(const string& str) const;
    int stringLen(const string& str) const;
    int hash(const string& key) const;

public:
    HospitalHashTable(int size = 20);
    ~HospitalHashTable();

    bool insert(const string& key, Hospital* hospitalPtr);
    Hospital* search(const string& key) const;
    bool remove(const string& key);
    void displayClean() const;
    void displayHospitalByID(const string& hospitalID) const;
    int getAllHospitals(Hospital** results, int maxResults) const;
    bool exists(const string& key) const;
    int getTotalCount() const;
};

#endif
