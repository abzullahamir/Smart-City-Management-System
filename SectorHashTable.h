#pragma once
#include "populationTree.h"  // This includes sectorNode
#include <iostream>
#include <string>

using namespace std;

struct SectorBucket {
    string sectorName;
    sectorNode* sectorPtr;
    SectorBucket* next;

    SectorBucket(string name = "", sectorNode* ptr = nullptr)
        : sectorName(name), sectorPtr(ptr), next(nullptr) {}
};

struct SectorHashTable {
    SectorBucket** arr;
    const int capacity = 101;  // Prime number for better distribution

    SectorHashTable() {
        arr = new SectorBucket * [capacity] {};
        for (int i = 0; i < capacity; i++) {
            arr[i] = nullptr;
        }
    }

    // Polynomial rolling hash for string sector names
    int hashFunction(string key) {
        long long hashVal = 0;
        int p = 31;  // Prime number for polynomial
        int m = capacity;
        long long pow = 1;

        for (char c : key) {
            // Ignore hyphens in sector names like "G-10"
            if (c != '-') {
                hashVal = (hashVal + (c)*pow) % m;
                pow = (pow * p) % m;
            }
        }

        return (hashVal % m + m) % m;
    }

    void insert(string key, sectorNode* obj) {
        int index = hashFunction(key);
        SectorBucket* newBucket = new SectorBucket(key, obj);

        if (arr[index] == nullptr) {
            arr[index] = newBucket;
        }
        else {
            SectorBucket* current = arr[index];
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newBucket;
        }
    }

    sectorNode* search(string key) {
        int index = hashFunction(key);
        SectorBucket* current = arr[index];

        while (current != nullptr) {
            if (current->sectorName == key) {
                return current->sectorPtr;
            }
            current = current->next;
        }
        return nullptr;
    }

    bool deleteKey(string key) {
        int index = hashFunction(key);
        SectorBucket* current = arr[index];
        SectorBucket* prev = nullptr;

        while (current != nullptr) {
            if (current->sectorName == key) {
                if (prev == nullptr) {
                    arr[index] = current->next;
                }
                else {
                    prev->next = current->next;
                }
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    // Display all sectors in hash table (for debugging)
    void displayAll() {
        cout << "\n--- Sector Hash Table Contents ---" << endl;
        int emptyBuckets = 0;
        int totalEntries = 0;

        for (int i = 0; i < capacity; i++) {
            if (arr[i] != nullptr) {
                cout << "Bucket " << i << ": ";
                SectorBucket* current = arr[i];
                while (current != nullptr) {
                    cout << current->sectorName << " -> ";
                    totalEntries++;
                    current = current->next;
                }
                cout << "NULL" << endl;
            }
            else {
                emptyBuckets++;
            }
        }
        cout << "Total sectors: " << totalEntries << endl;
        cout << "Empty buckets: " << emptyBuckets << " / " << capacity << endl;
    }

    ~SectorHashTable() {
        for (int i = 0; i < capacity; i++) {
            SectorBucket* current = arr[i];
            while (current != nullptr) {
                SectorBucket* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] arr;
    }
};