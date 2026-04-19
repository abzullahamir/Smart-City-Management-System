#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
using namespace std;

// Product in a mall
class Product {
private:
    string productID;
    string name;
    string category;
    double price;
    int quantity;

public:
    Product();
    Product(string id, string productName, string cat, double pr, int qty);

    // Getters
    string getProductID() const;
    string getName() const;
    string getCategory() const;
    double getPrice() const;
    int getQuantity() const;

    // Setters
    void setProductID(string id);
    void setName(string productName);
    void setCategory(string cat);
    void setPrice(double pr);
    void setQuantity(int qty);

    // Display
    void display() const;
};

#endif