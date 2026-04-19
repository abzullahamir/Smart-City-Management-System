#pragma once
#include "Education_Tree.h"

struct hashObject { // this will as a object in hash table linked list
	schoolNode* obj; //pointer to school object
	hashObject* next;
	string key; 

	hashObject(schoolNode* obj) {
		this->obj = obj;
		this->key = obj->schoolID; 
		this->next = nullptr;
	}
	hashObject() {
		this->obj = nullptr;
		this->key = "";
		this->next = nullptr;
	}
	
};


struct HastTable {
	hashObject** arr; // arr of schoolNode -> each schoolNode is head of linked list of departments
	const int  capacity = 101;

	HastTable() {
		arr = new hashObject * [capacity]{};
		for (int i = 0; i < capacity; i++) {
			arr[i] = nullptr;
		}
	}

	int hashFunction(string key) {
		long long hashVal = 0;
		int p = 53; // because our key has mix characters
		int m = capacity;
		long long pow = 1;

		for (char c : key) {
			//we are using original value of ascii
			hashVal = (hashVal + (c ) * pow) % m;

			pow = (pow * p) % m;
		}

		return (hashVal % m + m) % m; // to avoid negative values
	}

	void insert(string key, schoolNode* obj) {
		int index = hashFunction(key);
		hashObject* newObj = new hashObject(obj);
		if (arr[index] == nullptr) {
			arr[index] = newObj; 
			return;
		}
		else {
			hashObject* current = arr[index];
			while (current->next != nullptr) {
				current = current->next;
			}
			current->next = newObj;
		}
	}

	schoolNode* search(string key) {
		int index = hashFunction(key);
		hashObject* current = arr[index];
		while (current != nullptr) {
			if (current->key == key) {
				return current->obj;
			}
			current = current->next;
		}
		return nullptr; // not found
	}

	bool deleteKey(string key) {
		int index = hashFunction(key);
		hashObject* current = arr[index];
		hashObject* prev = nullptr;

		while (current != nullptr) {
			if (current->key == key) {
				if (prev == nullptr) {
					arr[index] = current->next;
				}
				else {
					prev->next = current->next;
				}
				delete current;
				return true; // deleted
			}
			prev = current;
			current = current->next;
		}
		return false; // not found
	}

	~HastTable() {
		for (int i = 0; i < capacity; i++) {
			hashObject* current = arr[i];
			while (current != nullptr) {
				hashObject* temp = current;
				current = current->next;
				delete temp;
			}
		}
		delete[] arr;
	}



};
