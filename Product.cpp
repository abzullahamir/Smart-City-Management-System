#include "Product.h"
#include <iostream>
using namespace std;

Product::Product() : productID(""), name(""), category(""), price(0.0), quantity(0) {}

Product::Product(string id, string productName, string cat, double pr, int qty)
    : productID(id), name(productName), category(cat), price(pr), quantity(qty) {
}

string Product::getProductID() const {
    return productID;
}

string Product::getName() const {
    return name;
}

string Product::getCategory() const {
    return category;
}

double Product::getPrice() const {
    return price;
}

int Product::getQuantity() const {
    return quantity;
}

void Product::setProductID(string id) {
    productID = id;
}

void Product::setName(string productName) {
    name = productName;
}

void Product::setCategory(string cat) {
    category = cat;
}

void Product::setPrice(double pr) {
    price = pr;
}

void Product::setQuantity(int qty) {
    quantity = qty;
}

void Product::display() const {
    cout << "  Product ID: " << productID << endl;
    cout << "  Name: " << name << endl;
    cout << "  Category: " << category << endl;
    cout << "  Price: Rs. " << price << endl;
    cout << "  Quantity: " << quantity << endl;
}