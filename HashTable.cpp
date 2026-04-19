#include "HashTable.h"

HashNode::HashNode(string k, Bus* v) : key(k), value(v), next(nullptr) {}

BusHashTable::BusHashTable(int size) : tableSize(size), itemCount(0) {
	table = new HashNode * [tableSize];
	for (int i = 0; i < tableSize; i++) {
		table[i] = nullptr;
	}
}

BusHashTable::~BusHashTable() {
	clear();
	delete[] table;
}

int BusHashTable::hashFunction(string key) {
	int hashValue = 0;

	for (int i = 0; i < key.length(); i++) {
		hashValue = (hashValue * 31 + key[i]) % tableSize; // Using Prime 31 to minimize chances of collision
		if (hashValue < 0) { // Handle negative values
			hashValue += tableSize;
		}
	}

	return hashValue;
}

void BusHashTable::insert(string key, Bus* value) {
	int index = hashFunction(key);

	// If key already exists
	HashNode* current = table[index];
	while (current) {
		if (current->key == key) {
			current->value = value; // Update
			return;
		}
		current = current->next;
	}

	// Insert at beginning of the chain
	HashNode* newNode = new HashNode(key, value);
	newNode->next = table[index];
	table[index] = newNode;
	itemCount++;
}

Bus* BusHashTable::search(string key) {
	int index = hashFunction(key);

	HashNode* current = table[index];
	while (current) {
		if (current->key == key) {
			return current->value;
		}
		current = current->next;
	}

	return nullptr;
}

bool BusHashTable::remove(string key) {
	int index = hashFunction(key);

	HashNode* current = table[index];
	HashNode* prev = nullptr;
	while (current) {
		if (current->key == key) {
			if (prev) {
				prev->next = current->next;
			}
			else {
				table[index] = current->next;
			}

			delete current;
			itemCount--;
			return true;
		}
		prev = current;
		current = current->next;
	}

	return false;
}

void BusHashTable::display() {
	cout << endl << "========== Bus Hash Table ==========" << endl;

	if (itemCount == 0) {
		cout << "  No buses registered." << endl;
		return;
	}

	for (int i = 0; i < tableSize; i++) {
		if (table[i]) {
			cout << "Bucket " << i << ": ";
			HashNode* current = table[i];
			while (current) {
				cout << "[" << current->key << "] ";
				current = current->next;
			}
			cout << endl;
		}
	}
	cout << "Total Buses: " << itemCount << endl;
}

int BusHashTable::getItemCount() const {
	return itemCount;
}

void BusHashTable::clear() {
	for (int i = 0; i < tableSize; i++) {
		HashNode* current = table[i];
		while (current) {
			HashNode* temp = current;
			current = current->next;
			delete temp;
		}
		table[i] = nullptr;
	}

	itemCount = 0;
}

Bus** BusHashTable::getAllBuses(int& count) {
	if (itemCount == 0) {
		count = 0;
		return nullptr;
	}

	// Array with all buses
	Bus** buses = new Bus * [itemCount];
	int index = 0;

	for (int i = 0; i < tableSize; i++) {
		HashNode* current = table[i];
		while (current) {
			buses[index++] = current->value;
			current = current->next;
		}
	}

	count = itemCount;
	return buses;
}