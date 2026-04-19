#pragma once
#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <string>
#include <iostream>

using namespace std;

struct Doctor {
    string name;
    string specialization;
    Doctor* next;

    Doctor() : name(""), specialization(""), next(nullptr) {}

    Doctor(string n, string spec)
        : name(n), specialization(spec), next(nullptr) {
    }
};

struct Patient {
    string name;
    int age;
    string condition;
    Patient* next;

    Patient() : name(""), age(0), condition(""), next(nullptr) {}

    Patient(string n, int a, string cond)
        : name(n), age(a), condition(cond), next(nullptr) {
    }
};

struct Hospital {
    string hospitalID;
    string name;
    string sector;
    int emergencyBeds;
    string specialization;

    Doctor* doctorHead;    // linked list of doctors
    Patient* patientHead;  // linked list of patients
    int doctorCount;
    int patientCount;
    double latitude;
    double longitude;

    Hospital()
        : hospitalID(""),
        name(""),
        sector(""),
        emergencyBeds(0),
        specialization(""),
        doctorHead(nullptr),
        patientHead(nullptr),
        doctorCount(0),
        patientCount(0),
        latitude(0.0),
        longitude(0.0) {
    }

    Hospital(string id, string n, string s,
        int eBeds, string spec,
        double lat, double lng)
        : hospitalID(id),
        name(n),
        sector(s),
        emergencyBeds(eBeds),
        specialization(spec),
        doctorHead(nullptr),
        patientHead(nullptr),
        doctorCount(0),
        patientCount(0),
        latitude(lat),
        longitude(lng) {
    }

    // add a doctor node at head
    void addDoctor(const Doctor& doc) {
        Doctor* newDoc = new Doctor(doc.name, doc.specialization);
        newDoc->next = doctorHead;
        doctorHead = newDoc;
        doctorCount++;
    }

    // add a patient node at head
    void addPatient(const Patient& pat) {
        Patient* newPat = new Patient(pat.name, pat.age, pat.condition);
        newPat->next = patientHead;
        patientHead = newPat;
        patientCount++;
    }

    void displayHospitalDetails() {
        cout << "\n=== " << name << " ===" << endl;
        if (!hospitalID.empty())
            cout << "ID: " << hospitalID << endl;
        if (!sector.empty())
            cout << "Sector: " << sector << endl;
        cout << "Emergency Beds: " << emergencyBeds << endl;
        if (!specialization.empty())
            cout << "Specialization: " << specialization << endl;
        cout << "Location: (" << latitude << ", " << longitude << ")\n";

        cout << "\nDoctors (" << doctorCount << "):" << endl;
        if (doctorHead == nullptr) {
            cout << "  No doctors currently listed.\n";
        }
        else {
            Doctor* d = doctorHead;
            while (d != nullptr) {
                cout << "  - Dr. " << d->name
                    << " (" << d->specialization << ")" << endl;
                d = d->next;
            }
        }

        cout << "\nPatients (" << patientCount << "):" << endl;
        if (patientHead == nullptr) {
            cout << "  No patients currently listed.\n";
        }
        else {
            Patient* p = patientHead;
            while (p != nullptr) {
                cout << "  - " << p->name
                    << " (Age " << p->age
                    << ", Condition: " << p->condition << ")" << endl;
                p = p->next;
            }
        }
    }
};

#endif // HOSPITAL_H
