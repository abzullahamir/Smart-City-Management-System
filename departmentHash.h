#pragma once
#include <string>
using namespace std;

struct departmentNode;

struct DepartmentBucket {
    string deptKey;            // e.g. department name  ("Computer Science")
    departmentNode* deptPtr;   // pointer into your linked list / tree
    DepartmentBucket* next;

    DepartmentBucket(string key = "", departmentNode* ptr = nullptr)
        : deptKey(key), deptPtr(ptr), next(nullptr) {}
};

struct DepartmentHashTable {
    DepartmentBucket** arr;
    const int capacity = 101;

    DepartmentHashTable() {
        arr = new DepartmentBucket * [capacity] {};
        for (int i = 0; i < capacity; i++) arr[i] = nullptr;
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

    void insert(const string& key, departmentNode* obj) {
        int index = hashFunction(key);

        // if key already exists, just update pointer
        DepartmentBucket* current = arr[index];
        while (current) {
            if (current->deptKey == key) {
                current->deptPtr = obj;
                return;
            }
            current = current->next;
        }

        DepartmentBucket* newObj = new DepartmentBucket(key, obj);
        newObj->next = arr[index];
        arr[index] = newObj;
    }

    departmentNode* search(const string& key) {
        int index = hashFunction(key);
        DepartmentBucket* current = arr[index];

        while (current) {
            if (current->deptKey == key) return current->deptPtr;
            current = current->next;
        }
        return nullptr;
    }

    bool deleteKey(const string& key) {
        int index = hashFunction(key);
        DepartmentBucket* current = arr[index];
        DepartmentBucket* prev = nullptr;

        while (current) {
            if (current->deptKey == key) {
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

    ~DepartmentHashTable() {
        for (int i = 0; i < capacity; i++) {
            DepartmentBucket* current = arr[i];
            while (current) {
                DepartmentBucket* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] arr;
    }
};
