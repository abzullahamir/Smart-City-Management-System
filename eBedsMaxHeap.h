#ifndef HOSPITAL_MAXHEAP_H
#define HOSPITAL_MAXHEAP_H

#include "hospitals.h"
#include <iostream>
#include <string>

using namespace std;

class HospitalMaxHeap {
private:
    Hospital** heap;  // array of pointers to hospital objects
    int capacity;
    int size;

    int parent(int i);
    int leftChild(int i);
    int rightChild(int i);

    void resize();
    void heapifyUp(int index);
    void heapifyDown(int index);
    string toLower(const string& s) const;

public:
    HospitalMaxHeap(int cap = 20);
    ~HospitalMaxHeap();

    void insert(Hospital* h);
    Hospital* extractMax();
    Hospital* peek();
    bool isEmpty();

    // rebuild heap when bed counts change
    void rebuildHeap();

    void display();
    void displaySortedByBeds(); // print in descending order using extractMax()


    // update specific hospital's bed count and restructure heap
    void updateBedCount(string hospitalID, int newBedCount);
};

#endif // HOSPITAL_MAXHEAP_H
