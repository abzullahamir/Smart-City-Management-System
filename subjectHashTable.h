#pragma once
#include "Education_Tree.h"

struct subjectBox {
	string subjectName;
	schoolNode * schoolPtr;
	subjectBox* next;
	subjectBox(string name = "", schoolNode* id = 0) : subjectName(name), schoolPtr(id), next(nullptr) {}


};


struct subjectHashTable {
	subjectBox** arr; // arr of subjectBox -> each subjectBox is head of linked list of schools
	const int  capacity = 101;

	subjectHashTable() {
		arr = new subjectBox * [capacity] {};
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
			hashVal = (hashVal + (c)*pow) % m;

			pow = (pow * p) % m;
		}

		return (hashVal % m + m) % m; // to avoid negative values
	}

	void insert(string key, schoolNode* obj) {
		int index = hashFunction(key);
		subjectBox* newObj = new subjectBox(key, obj);
		if (arr[index] == nullptr) {
			arr[index] = newObj;
			return;
		}
		else {
			subjectBox* current = arr[index];
			while (current->next != nullptr) {
				current = current->next;
			}
			current->next = newObj;
		}
	}

	schoolNode** search(string key, int& i) { // we will return array of schoolNode pointers which offers this subject
		int index = hashFunction(key);
		subjectBox* current = arr[index];
		
		if (current != nullptr) {
				// first count how many schools offer this subject
				int count = 0;
				subjectBox* temp = current;
				while (temp != nullptr) {
					if (temp->subjectName == key)
						count++;
					temp = temp->next;
				}

				// create an array to hold the pointers
				schoolNode** schoolsArray = new schoolNode * [count];
				i = 0;
				while (current != nullptr) {
					if (current->subjectName == key)
						schoolsArray[i++] = current->schoolPtr;
					current = current->next;
				}
				i = count;
				return schoolsArray;
		}
		return nullptr; // not found
	}

	bool deleteKey(string key) {
		int index = hashFunction(key);
		subjectBox* current = arr[index];
		subjectBox* prev = nullptr;

		while (current != nullptr) {
			if (current->subjectName == key) {
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
		return false; // not found
	}

	void displaySchoolsForSubject(string subjectKey) {
		int index = hashFunction(subjectKey);
		subjectBox* temp = arr[index];
		bool found = false;

		cout << "\n   ===================================================" << endl;
		cout << "    Subject Search Result: " << subjectKey << endl;
		cout << "   ===================================================" << endl;

		while (temp != nullptr) {
			
			// We must verify 
			if (temp->subjectName == subjectKey) {
				schoolNode* s = temp->schoolPtr;
				if (s != nullptr) {
					cout << "    -> [ID: " << s->schoolID << "] " << s->schoolName
						<< " (Rating: " << s->rating << ")" << endl;
					found = true;
				}
			}
			temp = temp->next;
		}

		if (!found) {
			cout << "    [!] No schools found offering \"" << subjectKey << "\"." << endl;
		}
		cout << "   ===================================================\n" << endl;
	}

	~subjectHashTable() {
		for (int i = 0; i < capacity; i++) {
			subjectBox* current = arr[i];
			while (current != nullptr) {
				subjectBox* temp = current;
				current = current->next;
				delete temp;
			}
		}
		delete[] arr;
	}
};