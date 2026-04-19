#ifndef PRODUCTHASHTABLE_H
#define PRODUCTHASHTABLE_H

#include "Product.h"

#include <iostream>
#include <string>
using namespace std;

class ProductHashNode {
public:
    string key;
    Product* value;
    ProductHashNode* next;

    ProductHashNode(string k, Product* v);
};

class ProductHashTable {
private:
    ProductHashNode** table;
    int tableSize;
    int itemCount;

    int hashFunction(string key);

public:
    ProductHashTable(int size = 101);
    ~ProductHashTable();

    void insert(string key, Product* value);
    Product* search(string key);
    bool remove(string key);
    void display();
    int getItemCount() const;
    void clear();

    // Get all products (for category search)
    Product** getAllProducts(int& count);

    void searchByCategory(string category);
};

#endif