#pragma once
#ifndef PUBLIC_FACILITY_H
#define PUBLIC_FACILITY_H

#include <iostream>
#include <string>

using namespace std;

struct PublicFacility {
    string facilityID;
    string name;
    string type;
    string sector;
    double latitude;
    double longitude;
    PublicFacility* next;

    PublicFacility()
        : facilityID(""),
        name(""),
        type(""),
        sector(""),
        latitude(0.0),
        longitude(0.0),
        next(nullptr) {
    }

    PublicFacility(const string& id, const string& n,
        const string& t, const string& s,
        double lat, double lon)
        : facilityID(id),
        name(n),
        type(t),
        sector(s),
        latitude(lat),
        longitude(lon),
        next(nullptr) {
    }

    void displayDetails() const {
        cout << "========================================\n";
        cout << "Facility ID: " << facilityID << "\n";
        cout << "Name: " << name << "\n";
        cout << "Type: " << type << "\n";
        cout << "Sector: " << sector << "\n";
        cout << "Location: (" << latitude << ", " << longitude << ")\n";
        cout << "========================================\n";
    }
};

#endif // PUBLIC_FACILITY_H
