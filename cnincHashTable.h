#pragma once
// Adjust the path to your actual familyTree.h location
#include "familyTree.h" 
#include <iostream>
#include <string>

using namespace std;

struct CNICBucket {
	string cnicKey;  
	personNode* personPtr;
	CNICBucket* next;     

	CNICBucket(string cnic = "", personNode* ptr = nullptr)
		: cnicKey(cnic), personPtr(ptr), next(nullptr) {}
};

struct CNICHashTable {
	CNICBucket** arr; 
	const int capacity = 101;

	CNICHashTable() {
		arr = new CNICBucket * [capacity] {};
		for (int i = 0; i < capacity; i++) {
			arr[i] = nullptr;
		}
	}

	int hashFunction(string key) {
		long long hashVal = 0;
		int p = 53; 
		int m = capacity;
		long long pow = 1;

		for (char c : key) {
			hashVal = (hashVal + (c)*pow) % m;
			pow = (pow * p) % m;
		}

		return (hashVal % m + m) % m;
	}

	void insert(string key, personNode* obj) {
		int index = hashFunction(key);
		CNICBucket* newObj = new CNICBucket(key, obj);

		if (arr[index] == nullptr) {
			arr[index] = newObj;
		}
		else {
			CNICBucket* current = arr[index];
			while (current->next != nullptr) {
				current = current->next;
			}
			current->next = newObj;
		}
	}

	personNode* search(string key) {
		int index = hashFunction(key);
		CNICBucket* current = arr[index];

		while (current != nullptr) {
			if (current->cnicKey == key) {
				return current->personPtr; 
			}
			current = current->next;
		}
		return nullptr; 
	}

	bool deleteKey(string key) {
		int index = hashFunction(key);
		CNICBucket* current = arr[index];
		CNICBucket* prev = nullptr;

		while (current != nullptr) {
			if (current->cnicKey == key) {
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

	
	void searchAndDisplay(string cnicKey) {
		personNode* result = search(cnicKey);

		cout << "\n   ===================================================" << endl;
		cout << "    CNIC Search Result: " << cnicKey << endl;
		cout << "   ===================================================" << endl;

		if (result != nullptr) {
			cout << "    -> Name: " << result->name << endl;
			cout << "    -> Occupation: " << result->occupation << endl;
			cout << "    -> Age: " << result->age << endl;
			result->displayFamilyTree();
		}
		else {
			cout << "    [!] No citizen found with CNIC: " << cnicKey << endl;
		}
		cout << "   ===================================================\n" << endl;
	}

	~CNICHashTable() {
		for (int i = 0; i < capacity; i++) {
			CNICBucket* current = arr[i];
			while (current != nullptr) {
				CNICBucket* temp = current;
				current = current->next;
				delete temp;
			}
		}
		delete[] arr;
	}
};