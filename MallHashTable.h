#ifndef MALLHASHTABLE_H
#define MALLHASHTABLE_H

#include "Mall.h"

#include <iostream>
#include <string>
using namespace std;

class MallHashNode {
public:
    string key;
    Mall* value;
    MallHashNode* next;

    MallHashNode(string k, Mall* v);
};

class MallHashTable {
private:
    MallHashNode** table;
    int tableSize;
    int itemCount;

    int hashFunction(string key);

public:
    MallHashTable(int size = 101);
    ~MallHashTable();

    void insert(string key, Mall* value);
    Mall* search(string key);
    bool remove(string key);
    void display();
    int getItemCount() const;
    void clear();

    // Get all malls (for nearest mall search)
    Mall** getAllMalls(int& count);
};

#endif