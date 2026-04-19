#pragma once
#include <string>
using namespace std;


struct classNode;

struct ClassBucket {
    string classKey;      // e.g. "CS-101"
    classNode* classPtr;  // pointer to classNode in dept's linked list
    ClassBucket* next;

    ClassBucket(string key = "", classNode* ptr = nullptr)
        : classKey(key), classPtr(ptr), next(nullptr) {}
};

struct ClassHashTable {
    ClassBucket** arr;
    const int capacity = 101;

    ClassHashTable() {
        arr = new ClassBucket * [capacity] {};
        for (int i = 0; i < capacity; ++i) arr[i] = nullptr;
    }

    int hashFunction(const string& key) {
        long long hashVal = 0;
        int p = 53;
        int m = capacity;
        long long pw = 1;

        for (char c : key) {
            hashVal = (hashVal + (c)*pw) % m;
            pw = (pw * p) % m;
        }
        return (hashVal % m + m) % m;
    }

    void insert(const string& key, classNode* obj) {
        int index = hashFunction(key);

        // update if key already exists
        ClassBucket* current = arr[index];
        while (current) {
            if (current->classKey == key) {
                current->classPtr = obj;
                return;
            }
            current = current->next;
        }

        // otherwise push new bucket at head
        ClassBucket* newObj = new ClassBucket(key, obj);
        newObj->next = arr[index];
        arr[index] = newObj;
    }

    classNode* search(const string& key) {
        int index = hashFunction(key);
        ClassBucket* current = arr[index];

        while (current) {
            if (current->classKey == key) return current->classPtr;
            current = current->next;
        }
        return nullptr;
    }

    bool deleteKey(const string& key) {
        int index = hashFunction(key);
        ClassBucket* current = arr[index];
        ClassBucket* prev = nullptr;

        while (current) {
            if (current->classKey == key) {
                if (!prev) arr[index] = current->next;
                else prev->next = current->next;
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    ~ClassHashTable() {
        for (int i = 0; i < capacity; ++i) {
            ClassBucket* current = arr[i];
            while (current) {
                ClassBucket* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] arr;
    }
};
