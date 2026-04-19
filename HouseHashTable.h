#pragma once
#include "populationTree.h"  // This includes houseNode
#include <iostream>

using namespace std;

struct HouseBucket {
    int houseNumber;  
    houseNode* housePtr;
    HouseBucket* next;     

    HouseBucket(int num = 0, houseNode* ptr = nullptr)
        : houseNumber(num), housePtr(ptr), next(nullptr) {}
};

struct HouseHashTable {
    HouseBucket** arr; 
    const int capacity = 101;  // Prime number for better distribution

    HouseHashTable() {
        arr = new HouseBucket * [capacity] {};
        for (int i = 0; i < capacity; i++) {
            arr[i] = nullptr;
        }
    }

    // Hash function for integer house numbers
    int hashFunction(int key) {
        // Using a different prime multiplier to avoid collisions
        long long hashVal = (key * 1618033989LL) % capacity;
        return hashVal;
    }

    void insert(int key, houseNode* obj) {
        int index = hashFunction(key);
        HouseBucket* newBucket = new HouseBucket(key, obj);

        if (arr[index] == nullptr) {
            arr[index] = newBucket;
        }
        else {
            HouseBucket* current = arr[index];
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newBucket;
        }
    }

    houseNode* search(int key) {
        int index = hashFunction(key);
        HouseBucket* current = arr[index];

        while (current != nullptr) {
            if (current->houseNumber == key) {
                return current->housePtr; 
            }
            current = current->next;
        }
        return nullptr; 
    }

    bool deleteKey(int key) {
        int index = hashFunction(key);
        HouseBucket* current = arr[index];
        HouseBucket* prev = nullptr;

        while (current != nullptr) {
            if (current->houseNumber == key) {
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

    // Display all houses in hash table (for debugging)
    void displayAll() {
        cout << "\n--- House Hash Table Contents ---" << endl;
        int emptyBuckets = 0;
        int totalEntries = 0;
        
        for (int i = 0; i < capacity; i++) {
            if (arr[i] != nullptr) {
                cout << "Bucket " << i << ": ";
                HouseBucket* current = arr[i];
                while (current != nullptr) {
                    cout << "H-" << current->houseNumber << " -> ";
                    totalEntries++;
                    current = current->next;
                }
                cout << "NULL" << endl;
            } else {
                emptyBuckets++;
            }
        }
        cout << "Total houses: " << totalEntries << endl;
        cout << "Empty buckets: " << emptyBuckets << " / " << capacity << endl;
    }

    ~HouseHashTable() {
        for (int i = 0; i < capacity; i++) {
            HouseBucket* current = arr[i];
            while (current != nullptr) {
                HouseBucket* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] arr;
    }
};