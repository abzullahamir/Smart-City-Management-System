#include "MallHashTable.h"

MallHashNode::MallHashNode(string k, Mall* v) : key(k), value(v), next(nullptr) {}

MallHashTable::MallHashTable(int size) : tableSize(size), itemCount(0) {
    table = new MallHashNode * [tableSize];
    for (int i = 0; i < tableSize; i++) {
        table[i] = nullptr;
    }
}

MallHashTable::~MallHashTable() {
    clear();
    delete[] table;
}

int MallHashTable::hashFunction(string key) {
    int hashValue = 0;

    for (int i = 0; i < key.length(); i++) {
        hashValue = (hashValue * 31 + key[i]) % tableSize;
        if (hashValue < 0) hashValue += tableSize;
    }

    return hashValue;
}

void MallHashTable::insert(string key, Mall* value) {
    int index = hashFunction(key);

    // If key already exists
    MallHashNode* current = table[index];
    while (current) {
        if (current->key == key) {
            current->value = value; // Update
            return;
        }
        current = current->next;
    }

    // Insert at beginning of chain
    MallHashNode* newNode = new MallHashNode(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    itemCount++;
}

Mall* MallHashTable::search(string key) {
    int index = hashFunction(key);

    MallHashNode* current = table[index];
    while (current) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }

    return nullptr;
}

bool MallHashTable::remove(string key) {
    int index = hashFunction(key);

    MallHashNode* current = table[index];
    MallHashNode* prev = nullptr;
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

void MallHashTable::display() {
    cout << endl << "========== All Malls ==========" << endl;

    if (itemCount == 0) {
        cout << "  No malls registered." << endl;
        return;
    }

    for (int i = 0; i < tableSize; i++) {
        if (table[i]) {
            cout << "Bucket " << i << ": ";
            MallHashNode* current = table[i];
            while (current) {
                cout << "[" << current->key << "] ";
                current = current->next;
            }
            cout << endl;
        }
    }
    cout << "Total Malls: " << itemCount << endl;
}

int MallHashTable::getItemCount() const {
    return itemCount;
}

void MallHashTable::clear() {
    for (int i = 0; i < tableSize; i++) {
        MallHashNode* current = table[i];
        while (current) {
            MallHashNode* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
    itemCount = 0;
}

Mall** MallHashTable::getAllMalls(int& count) {
    if (itemCount == 0) {
        count = 0;
        return nullptr;
    }

    Mall** malls = new Mall * [itemCount];
    int index = 0;

    for (int i = 0; i < tableSize; i++) {
        MallHashNode* current = table[i];
        while (current) {
            malls[index++] = current->value;
            current = current->next;
        }
    }

    count = itemCount;
    return malls;
}