#pragma once
#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>
using namespace std;

struct Medicine {
    string medicineName;
    string formula;
    int price;

    Medicine()
        : medicineName(""), formula(""), price(0) {
    }

    Medicine(const string& name, const string& form, int p)
        : medicineName(name), formula(form), price(p) {
    }
};

#endif // MEDICINE_H
