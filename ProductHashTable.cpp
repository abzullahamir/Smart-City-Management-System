#include "ProductHashTable.h"

ProductHashNode::ProductHashNode(string k, Product* v) : key(k), value(v), next(nullptr) {}

ProductHashTable::ProductHashTable(int size) : tableSize(size), itemCount(0) {
    table = new ProductHashNode * [tableSize];
    for (int i = 0; i < tableSize; i++) {
        table[i] = nullptr;
    }
}

ProductHashTable::~ProductHashTable() {
    clear();
    delete[] table;
}

int ProductHashTable::hashFunction(string key) {
    int hashValue = 0;

    for (int i = 0; i < key.length(); i++) {
        hashValue = (hashValue * 31 + key[i]) % tableSize; // Using Prime 31 to minimize chances of collision
        if (hashValue < 0) { // Handle negative values
            hashValue += tableSize;
        }
    }

    return hashValue;
}

void ProductHashTable::insert(string key, Product* value) {
    int index = hashFunction(key);

    // If key already exists
    ProductHashNode* current = table[index];
    while (current) {
        if (current->key == key) {
            current->value = value; // Update
            return;
        }
        current = current->next;
    }

    // Insert at beginning of chain
    ProductHashNode* newNode = new ProductHashNode(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    itemCount++;
}

Product* ProductHashTable::search(string key) {
    int index = hashFunction(key);

    ProductHashNode* current = table[index];
    while (current) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }

    return nullptr;
}

bool ProductHashTable::remove(string key) {
    int index = hashFunction(key);

    ProductHashNode* current = table[index];
    ProductHashNode* prev = nullptr;
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

void ProductHashTable::display() {
    cout << endl << "========== Product Hash Table ==========" << endl;

    if (itemCount == 0) {
        cout << "  No products available." << endl;
        return;
    }

    for (int i = 0; i < tableSize; i++) {
        if (table[i]) {
            cout << "Bucket " << i << ": ";
            ProductHashNode* current = table[i];
            while (current) {
                cout << "[" << current->key << "] ";
                current = current->next;
            }
            cout << endl;
        }
    }
    cout << "Total Products: " << itemCount << endl;
}

int ProductHashTable::getItemCount() const {
    return itemCount;
}

void ProductHashTable::clear() {
    for (int i = 0; i < tableSize; i++) {
        ProductHashNode* current = table[i];
        while (current) {
            ProductHashNode* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
    itemCount = 0;
}

Product** ProductHashTable::getAllProducts(int& count) {
    if (itemCount == 0) {
        count = 0;
        return nullptr;
    }

    Product** products = new Product * [itemCount];
    int index = 0;

    for (int i = 0; i < tableSize; i++) {
        ProductHashNode* current = table[i];
        while (current) {
            products[index++] = current->value;
            current = current->next;
        }
    }

    count = itemCount;
    return products;
}

void ProductHashTable::searchByCategory(string category) {
    bool found = false;

    for (int i = 0; i < tableSize; i++) {
        ProductHashNode* current = table[i];
        while (current) {
            if (current->value && current->value->getCategory() == category) {
                if (!found) {
                    cout << endl << "Products in category '" << category << "':" << endl;
                    cout << "----------------------------------------" << endl;
                    found = true;
                }
                current->value->display();
                cout << endl;
            }
            current = current->next;
        }
    }

    if (!found) {
        cout << "No products found in category '" << category << "'." << endl;
    }
}