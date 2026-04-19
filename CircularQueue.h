#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include "Passenger.h"
#include <iostream>
using namespace std;

// Circular Queue for passenger management at bus stops
class CircularQueue {
private:
    Passenger* queue;
    int front;
    int rear;
    int capacity;
    int currentSize;

public:
    CircularQueue(int cap = 50);
    ~CircularQueue();

    bool enqueue(Passenger p);
    Passenger dequeue();
    Passenger peek();
    bool isEmpty() const;
    bool isFull() const;
    int size() const;
    void display() const;
    void clear();
};

#endif