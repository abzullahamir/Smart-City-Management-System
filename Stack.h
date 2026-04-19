#ifndef STACK_H
#define STACK_H

#include <string>
#include <iostream>
using namespace std;

// Node for travel history
class TravelHistoryNode {
public:
    string stopID;
    string stopName;
    string timestamp;
    TravelHistoryNode* next;

    TravelHistoryNode(string id, string name, string time);
};

// Stack for travel history
class Stack {
private:
    TravelHistoryNode* top;
    int size;
    int maxSize;

public:
    Stack(int max = 100);
    ~Stack();

    void push(string stopID, string stopName, string timestamp);
    TravelHistoryNode* pop();
    TravelHistoryNode* peek();
    bool isEmpty() const;
    bool isFull() const;
    int getSize() const;
    void display() const;
    void clear();
};

#endif