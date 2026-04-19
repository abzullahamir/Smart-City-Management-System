#include "Mall.h"
#include <iostream>
using namespace std;

// CategoryList Implementation
CategoryList::CategoryList(int cap) : count(0), capacity(cap) {
    categories = new string[capacity];
}

CategoryList::~CategoryList() {
    delete[] categories;
}

void CategoryList::add(string category) {
    if (!contains(category) && count < capacity) {
        categories[count++] = category;
    }
}

bool CategoryList::contains(string category) {
    for (int i = 0; i < count; i++) {
        if (categories[i] == category) {
            return true;
        }
    }
    return false;
}

void CategoryList::display() {
    for (int i = 0; i < count; i++) {
        cout << (i + 1) << ". " << categories[i] << endl;
    }
}

Mall::Mall() : mallID(""), name(""), location(""), latitude(0.0), longitude(0.0) {
    products = new ProductHashTable(101);
}

Mall::Mall(string id, string mallName, string loc, double lat, double lon)
    : mallID(id), name(mallName), location(loc), latitude(lat), longitude(lon) {
    products = new ProductHashTable(101);
}

Mall::~Mall() {
    delete products;
}

string Mall::getMallID() const {
    return mallID;
}

string Mall::getName() const {
    return name;
}

string Mall::getLocation() const {
    return location;
}

double Mall::getLatitude() const {
    return latitude;
}

double Mall::getLongitude() const {
    return longitude;
}

ProductHashTable* Mall::getProducts() const {
    return products;
}

void Mall::setMallID(string id) {
    mallID = id;
}

void Mall::setName(string mallName) {
    name = mallName;
}

void Mall::setLocation(string loc) {
    location = loc;
}

void Mall::setCoordinates(double lat, double lon) {
    latitude = lat;
    longitude = lon;
}

void Mall::addProduct(Product* product) {
    products->insert(product->getProductID(), product); // key + product*
}

Product* Mall::searchProduct(string productID) {
    return products->search(productID);
}

void Mall::removeProduct(string productID) {
    products->remove(productID);
}

void Mall::displayProducts() {
    cout << endl << "===== Products in " << name << " =====" << endl;
    if (products->getItemCount() == 0) {
        cout << "No products available." << endl;
    }
    else {
        products->display();
    }
}

void Mall::searchProductsByCategory(string category) {
    products->searchByCategory(category);
}

int Mall::getProductCount() const {
    return products->getItemCount();
}

CategoryList* Mall::getAvailableCategories() {
    CategoryList* categoryList = new CategoryList(50);

    int productCount = 0;
    Product** allProducts = products->getAllProducts(productCount);

    if (allProducts) {
        for (int i = 0; i < productCount; i++) {
            categoryList->add(allProducts[i]->getCategory());
        }
        delete[] allProducts;
    }

    return categoryList;
}

void Mall::display() const {
    cout << endl << "----- Mall Details -----" << endl;
    cout << "Mall ID: " << mallID << endl;
    cout << "Name: " << name << endl;
    cout << "Location: " << location << endl;
    cout << "Coordinates: (" << latitude << ", " << longitude << ")" << endl;
    cout << "Total Products: " << products->getItemCount() << endl;
}