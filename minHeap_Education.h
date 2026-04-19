#pragma once
#include "Education_Tree.h"

class minHeap_Education {
	schoolNode** heapArray;
	int size; // current size -> we will dynammically create new arr to insert new elements
	int capacity; // maximum capacity

	public:
	minHeap_Education() {
		heapArray = new schoolNode * [20]; // initial capacity of 10
		size = 0;
		capacity = 10;

	}
	int getSize() const { return size; }

	void heapify(int i) {
		int min = i;
		int left = 2 * i + 1;
		int right = 2 * i + 2;

		if ( left < size && heapArray[left]->rating < heapArray[min]->rating)
			min = left;
		if ( right < size && heapArray[right]->rating < heapArray[min]->rating)
			min = right;

		if (min != i) {
			schoolNode * temp = heapArray[i];
			heapArray[i] = heapArray[min];
			heapArray[min] = temp;
			
			heapify(min);
		}
	}

	void heapifyUp(int i) {
		// size of parent is more than the child swap
		while (i > 0 && heapArray[(i - 1)/ 2]->rating > heapArray[i]->rating) {
			schoolNode* temp = heapArray[i];
			heapArray[i] = heapArray[(i - 1) / 2];
			heapArray[(i - 1) / 2] = temp;
			i = (i - 1) / 2;
		}
	}

	void insert(schoolNode * obj) {
		//resize
		if (size == capacity) {
			capacity *= 2; // Double the size
			schoolNode** newArray = new schoolNode * [capacity];
			for (int i = 0; i < size; i++) {
				newArray[i] = heapArray[i];
			}
			delete[] heapArray;
			heapArray = newArray;
		}

		// 2. Insert at the end
		heapArray[size] = obj;

		
		heapifyUp(size);

		size++;

	}

	schoolNode* extractMin() {
		if (size == 0)
			return nullptr;

		schoolNode* min = heapArray[0];
		heapArray[0] = heapArray[size - 1];
		size--;
		heapify(0);
		return min;
	}

	schoolNode* extractMax() {
		if (size == 0)
			return nullptr;

		// Find the maximum element in the heap
		int maxIndex = 0;
		for (int i = 1; i < size; i++) {
			if (heapArray[i]->rating > heapArray[maxIndex]->rating) {
				maxIndex = i;
			}
		}

		schoolNode* max = heapArray[maxIndex];
		heapArray[maxIndex] = heapArray[size - 1];
		size--;
		heapify(maxIndex);
		return max;
	}

	void deleteKey ( int i){
		if (i < 0 || i >= size)
			return;

		heapArray[i] = heapArray[size - 1];
		size--;
		heapify(i);
	}

	schoolNode * getMin() {
		if (size == 0)
			return nullptr;
		return heapArray[0];
	}

	schoolNode* getMax() {
		if (size == 0)
			return nullptr;

		// Find the maximum element in the heap
		int maxIndex = 0;
		for (int i = 1; i < size; i++) {
			if (heapArray[i]->rating > heapArray[maxIndex]->rating) {
				maxIndex = i;
			}
		}
		return heapArray[maxIndex];
	}

	void printLowestSchool() {
		schoolNode* s = getMin();
		if (!s) {
			cout << "No schools in heap.\n";
			return;
		}
		cout << "\n=== Lowest Rated School (Min-Heap) ===\n";
		cout << "ID     : " << s->schoolID << endl;
		cout << "Name   : " << s->schoolName << endl;
		cout << "Sector : " << s->sector << endl;
		cout << "Rating : " << s->rating << endl;
	}

	
	void printSchoolsAscending() {
		if (size == 0) {
			cout << "No schools in heap.\n";
			return;
		}

		// Build a temporary heap so we don't destroy the original
		minHeap_Education temp;
		for (int i = 0; i < size; ++i) {
			temp.insert(heapArray[i]);      // re-insert pointers into temp heap
		}

		cout << "\n=== Schools in Ascending Order of Rating (Min-Heap) ===\n";
		int rank = 1;
		while (temp.getSize() > 0) {
			schoolNode* s = temp.extractMin();   // smallest rating
			if (s) {
				cout << rank++ << ". "
					<< s->schoolName << " (ID: " << s->schoolID
					<< ", Rating: " << s->rating << ")\n";
			}
		}
	}


	~minHeap_Education() {
		delete[] heapArray;
	}

};