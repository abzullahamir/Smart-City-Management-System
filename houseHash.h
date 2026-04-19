#pragma once
#pragma once
#include "populationTree.h" 
#include <string>
#include <iostream>

using namespace std;

struct HouseBucket {
    string uniqueKey;     // Composite Key: "Sector-Street-House"
    houseNode* housePtr;
    HouseBucket* next;

    HouseBucket(string key, houseNode* ptr) : uniqueKey(key), housePtr(ptr), next(nullptr) {}
};

struct HouseHashTable {
    HouseBucket** arr;
    const int capacity = 101;

    HouseHashTable() {
        arr = new HouseBucket * [capacity] {};
    }

    // Hash Function for String Keys
    int hashFunction(string key) {
        long long hashVal = 0;
        int p = 53;
        int m = capacity;
        long long pow = 1;

        for (char c : key) {
            hashVal = (hashVal + c * pow) % m;
            pow = (pow * p) % m;
        }
        return (hashVal % m + m) % m;
    }

    // Helper to generate the unique composite key
    string generateKey(string sector, int street, int house) {
      //  "G-10-S5-H12"
        return sector + "-S" + to_string(street) + "-H" + to_string(house);
    }

    void insert(string sector, int street, int house, houseNode* obj) {
        string key = generateKey(sector, street, house);
        int index = hashFunction(key);

        HouseBucket* newObj = new HouseBucket(key, obj);

        if (arr[index] == nullptr) {
            arr[index] = newObj;
        }
        else {
            HouseBucket* temp = arr[index];
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newObj;
        }
    }

    houseNode* search(string sector, int street, int house) {
        string key = generateKey(sector, street, house);
        int index = hashFunction(key);
        HouseBucket* temp = arr[index];

        while (temp != nullptr) {
            if (temp->uniqueKey == key) {
                return temp->housePtr;
            }
            temp = temp->next;
        }
        return nullptr; // Not found
    }

    ~HouseHashTable() {
		for (int i = 0; i < capacity; i++) {
			HouseBucket* temp = arr[i];
			while (temp != nullptr) {
				HouseBucket* toDelete = temp;
				temp = temp->next;
				delete toDelete;
			}
		}
		delete[] arr;
	}
};