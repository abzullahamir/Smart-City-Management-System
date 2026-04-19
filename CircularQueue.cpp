#include "CircularQueue.h"

CircularQueue::CircularQueue(int cap) : capacity(cap), front(0), rear(-1), currentSize(0) {
    queue = new Passenger[capacity];
}

CircularQueue::~CircularQueue() {
    delete[] queue;
}

bool CircularQueue::enqueue(Passenger p) {
    if (isFull()) {
        cout << "Queue is full! Cannot add passenger." << endl;
        return false;
    }

    rear = (rear + 1) % capacity;
    queue[rear] = p;
    currentSize++;

    return true;
}

Passenger CircularQueue::dequeue() {
    if (isEmpty()) {
        cout << "Queue is empty! No passengers to dequeue." << endl;
        return Passenger();
    }

    Passenger p = queue[front];
    front = (front + 1) % capacity;
    currentSize--;

    return p;
}

Passenger CircularQueue::peek() {
    if (isEmpty()) {
        return Passenger();
    }
    return queue[front];
}

bool CircularQueue::isEmpty() const {
    if (currentSize == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool CircularQueue::isFull() const {
    if (currentSize == capacity) {
        return true;
    }
    else {
        return false;
    }
}

int CircularQueue::size() const {
    return currentSize;
}

void CircularQueue::display() const {
    if (isEmpty()) {
        cout << "Queue is empty." << endl;
        return;
    }

    cout << "Passengers in queue (" << currentSize << "/" << capacity << "):" << endl;

    int index = front;
    for (int i = 0; i < currentSize; i++) {
        queue[index].display();
        index = (index + 1) % capacity;
    }
}

void CircularQueue::clear() {
    front = 0;
    rear = -1;
    currentSize = 0;
}