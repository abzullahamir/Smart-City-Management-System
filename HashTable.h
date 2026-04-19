#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <string>
using namespace std;

class Bus;

// Hash Node for separate chaining
class HashNode {
public:
    string key;
    Bus* value;
    HashNode* next;

    HashNode(string k, Bus* v);
};

// Hash Table with separate chaining
class BusHashTable {
private:
    HashNode** table;
    int tableSize;
    int itemCount;

    // Hash function (Polynomial rolling hash)
    int hashFunction(string key);

public:
    BusHashTable(int size = 101);
    ~BusHashTable();

    void insert(string key, Bus* value);
    Bus* search(string key);
    bool remove(string key);
    void display();
    int getItemCount() const;
    void clear();

    // Searching through all buses
    Bus** getAllBuses(int& count); 
};

#endif