#include "Stack.h"
#include <ctime>

TravelHistoryNode::TravelHistoryNode(string id, string name, string time) : stopID(id), stopName(name), timestamp(time), next(nullptr) {}

Stack::Stack(int max) : top(nullptr), size(0), maxSize(max) {}

Stack::~Stack() {
    clear();
}

void Stack::push(string stopID, string stopName, string timestamp) {
    if (isFull()) {
        cout << "Stack is full! Cannot add more history." << endl;
        return;
    }

    TravelHistoryNode* newNode = new TravelHistoryNode(stopID, stopName, timestamp);
    newNode->next = top;
    top = newNode;
    size++;
}

TravelHistoryNode* Stack::pop() {
    if (isEmpty()) {
        cout << "Stack is empty! No history to pop." << endl;
        return nullptr;
    }

    TravelHistoryNode* temp = top;
    top = top->next;
    size--;

    temp->next = nullptr;
    return temp;
}

TravelHistoryNode* Stack::peek() {
    if (isEmpty()) {
        return nullptr;
    }
    return top;
}

bool Stack::isEmpty() const {
    if (top == nullptr) {
        return true;
    }
    else {
        return false;
    }
}

bool Stack::isFull() const {
    if (size >= maxSize) {
        return true;
    }
    else {
        return false;
    }
}

int Stack::getSize() const {
    return size;
}

void Stack::display() const {
    if (isEmpty()) {
        cout << "No travel history." << endl;
        return;
    }

    cout << endl << "===== Travel History (Most Recent First) =====" << endl;
    TravelHistoryNode* current = top;
    int count = 1;

    while (current) {
        cout << count++ << ". [" << current->timestamp << "] " << current->stopID << " - " << current->stopName << endl;
        current = current->next;
    }
    cout << "Total stops visited: " << size << endl;
}

void Stack::clear() {
    while (!isEmpty()) {
        TravelHistoryNode* temp = pop();
        delete temp;
    }
}