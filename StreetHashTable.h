#pragma once
#include "populationTree.h"  // This includes streetNode
#include <iostream>

using namespace std;

struct StreetBucket {
    int streetNumber;
    streetNode* streetPtr;
    StreetBucket* next;

    StreetBucket(int num = 0, streetNode* ptr = nullptr)
        : streetNumber(num), streetPtr(ptr), next(nullptr) {}
};

struct StreetHashTable {
    StreetBucket** arr;
    const int capacity = 101;  // Prime number for better distribution

    StreetHashTable() {
        arr = new StreetBucket * [capacity] {};
        for (int i = 0; i < capacity; i++) {
            arr[i] = nullptr;
        }
    }

    // Hash function for integer street numbers
    int hashFunction(int key) {
        // Using multiplication with a prime and modulo
        long long hashVal = (key * 2654435761LL) % capacity;
        return hashVal;
    }

    void insert(int key, streetNode* obj) {
        int index = hashFunction(key);
        StreetBucket* newBucket = new StreetBucket(key, obj);

        if (arr[index] == nullptr) {
            arr[index] = newBucket;
        }
        else {
            StreetBucket* current = arr[index];
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newBucket;
        }
    }

    streetNode* search(int key) {
        int index = hashFunction(key);
        StreetBucket* current = arr[index];

        while (current != nullptr) {
            if (current->streetNumber == key) {
                return current->streetPtr;
            }
            current = current->next;
        }
        return nullptr;
    }

    bool deleteKey(int key) {
        int index = hashFunction(key);
        StreetBucket* current = arr[index];
        StreetBucket* prev = nullptr;

        while (current != nullptr) {
            if (current->streetNumber == key) {
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

    // Display all streets in hash table (for debugging)
    void displayAll() {
        cout << "\n--- Street Hash Table Contents ---" << endl;
        int emptyBuckets = 0;
        int totalEntries = 0;

        for (int i = 0; i < capacity; i++) {
            if (arr[i] != nullptr) {
                cout << "Bucket " << i << ": ";
                StreetBucket* current = arr[i];
                while (current != nullptr) {
                    cout << "St-" << current->streetNumber << " -> ";
                    totalEntries++;
                    current = current->next;
                }
                cout << "NULL" << endl;
            }
            else {
                emptyBuckets++;
            }
        }
        cout << "Total streets: " << totalEntries << endl;
        cout << "Empty buckets: " << emptyBuckets << " / " << capacity << endl;
    }

    ~StreetHashTable() {
        for (int i = 0; i < capacity; i++) {
            StreetBucket* current = arr[i];
            while (current != nullptr) {
                StreetBucket* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] arr;
    }
};