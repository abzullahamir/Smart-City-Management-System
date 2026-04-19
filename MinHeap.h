#ifndef MINHEAP_H
#define MINHEAP_H

#include <iostream>
#include <string>
using namespace std;

// Heap element for Dijkstra's Algorithm
class HeapNode {
public:
	string stopID;
	double distance;

	HeapNode();
	HeapNode(string id, double dist);
};

// Min Heap for priority queue
class MinHeap {
private:
	HeapNode* heap;
	int capacity;
	int size;

	int parent(int i);
	int leftChild(int i);
	int rightChild(int i);
	void heapifyUp(int index);
	void heapifyDown(int index);
	void swap(int i, int j);

public:
	MinHeap(int cap = 1000);
	~MinHeap();

	void insert(string stopID, double distance);
	HeapNode extractMin();
	bool isEmpty() const;
	int getSize() const;
	void clear();
};

#endif