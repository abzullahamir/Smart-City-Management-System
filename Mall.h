#ifndef MALL_H
#define MALL_H

#include "ProductHashTable.h"

#include <string>
using namespace std;

// Simple string array helper for categories
class CategoryList {
public:
    string* categories;
    int count;
    int capacity;

    CategoryList(int cap = 50);
    ~CategoryList();

    void add(string category);
    bool contains(string category);
    void display();
};

class Mall {
private:
    string mallID;
    string name;
    string location;
    double latitude;
    double longitude;
    ProductHashTable* products;

public:
    Mall();
    Mall(string id, string mallName, string loc, double lat, double lon);
    ~Mall();

    // Getters
    string getMallID() const;
    string getName() const;
    string getLocation() const;
    double getLatitude() const;
    double getLongitude() const;
    ProductHashTable* getProducts() const;

    // Setters
    void setMallID(string id);
    void setName(string mallName);
    void setLocation(string loc);
    void setCoordinates(double lat, double lon);

    // Product management
    void addProduct(Product* product);
    Product* searchProduct(string productID);
    void removeProduct(string productID);
    void displayProducts();
    void searchProductsByCategory(string category);
    int getProductCount() const;

    CategoryList* getAvailableCategories();

    void display() const;
};

#endif