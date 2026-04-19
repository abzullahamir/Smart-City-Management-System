#include "MinHeap.h"

HeapNode::HeapNode() : stopID(""), distance(0) {}

HeapNode::HeapNode(string id, double dist) : stopID(id), distance(dist) {}

MinHeap::MinHeap(int cap) : capacity(cap), size(0) {
	heap = new HeapNode[capacity];
}

MinHeap::~MinHeap() {
	delete[] heap;
}

int MinHeap::parent(int i) {
	return (i - 1) / 2;
}

int MinHeap::leftChild(int i) {
	return 2 * i + 1;
}

int MinHeap::rightChild(int i) {
	return 2 * i + 2;
}

void MinHeap::swap(int i, int j) {
	HeapNode temp = heap[i];
	heap[i] = heap[j];
	heap[j] = temp;
}

void MinHeap::heapifyUp(int index) {
	while (index > 0 && heap[parent(index)].distance > heap[index].distance) {
		swap(index, parent(index));
		index = parent(index);
	}
}

void MinHeap::heapifyDown(int index) {
	int minIndex = index;
	int left = leftChild(index);
	int right = rightChild(index);
	
	if (left < size && heap[left].distance < heap[minIndex].distance) {
		minIndex = left;
	}

	if (right < size && heap[right].distance < heap[minIndex].distance) {
		minIndex = right;
	}

	if (minIndex != index) {
		swap(index, minIndex);
		heapifyDown(minIndex);
	}
}

void MinHeap::insert(string stopID, double distance) {
	if (size >= capacity) {
		cout << "Heap is full!" << endl;
		return;
	}

	heap[size] = HeapNode(stopID, distance);
	heapifyUp(size);
	size++;
}

HeapNode MinHeap::extractMin() {
	if (isEmpty()) {
		return HeapNode("", -1);
	}

	HeapNode minNode = heap[0];
	heap[0] = heap[size - 1];
	size--;
	heapifyDown(0);

	return minNode;
}

bool MinHeap::isEmpty() const {
	return size == 0;
}

int MinHeap::getSize() const {
	return size;
}

void MinHeap::clear() {
	size = 0;
}

