#pragma once
#ifndef DOCTOR_HASHTABLE_H
#define DOCTOR_HASHTABLE_H

#include <iostream>
#include <string>
#include "hospitals.h"

using namespace std;

struct DoctorHashNode {
    string key;
    Doctor* doctor;
    Hospital* hospital;
    DoctorHashNode* next;

    DoctorHashNode() : doctor(nullptr), hospital(nullptr), next(nullptr) {}
    DoctorHashNode(string k, Doctor* d, Hospital* h)
        : key(k), doctor(d), hospital(h), next(nullptr) {
    }
};

class DoctorHashTable {
private:
    int tableSize;
    DoctorHashNode** table;

    int stringLen(const string& str) const;
    string toLower(const string& str) const;
    int hash(const string& key) const;

public:
    DoctorHashTable(int size = 20);
    ~DoctorHashTable();

    bool insert(const string& specialization, Doctor* doctorPtr, Hospital* hospitalPtr);
    void searchBySpecialization(const string& specialization) const;
    bool removeDoctor(const string& doctorName, const string& hospitalID);
    void displayClean() const;
    int getDoctorsBySpecialization(const string& specialization, Doctor** outDoctors,
        Hospital** outHospitals, int maxResults) const;
    bool doctorExists(const string& doctorName, const string& hospitalID) const;
};

#endif
