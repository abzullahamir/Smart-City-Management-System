#include "eBedsMaxHeap.h"

using namespace std;

// ============ helper functions ============

int HospitalMaxHeap::parent(int i) {
    return (i - 1) / 2;
}

int HospitalMaxHeap::leftChild(int i) {
    return 2 * i + 1;
}

int HospitalMaxHeap::rightChild(int i) {
    return 2 * i + 2;
}

string HospitalMaxHeap::toLower(const string& s) const {
    string res = s;
    for (size_t i = 0; i < res.length(); i++) {
        if (res[i] >= 'A' && res[i] <= 'Z') {
            res[i] = static_cast<char>(res[i] + 32);
        }
    }
    return res;
}

void HospitalMaxHeap::resize() {
    int newCapacity = (capacity > 0 ? capacity * 2 : 4);
    Hospital** newHeap = new Hospital * [newCapacity];
    for (int i = 0; i < size; i++) {
        newHeap[i] = heap[i];
    }
    delete[] heap;
    heap = newHeap;
    capacity = newCapacity;
}

void HospitalMaxHeap::heapifyUp(int index) {
    while (index > 0 &&
        heap[parent(index)]->emergencyBeds < heap[index]->emergencyBeds) {
        Hospital* temp = heap[index];
        heap[index] = heap[parent(index)];
        heap[parent(index)] = temp;
        index = parent(index);
    }
}

void HospitalMaxHeap::heapifyDown(int index) {
    while (true) {
        int maxIndex = index;
        int left = leftChild(index);
        int right = rightChild(index);

        if (left < size &&
            heap[left]->emergencyBeds > heap[maxIndex]->emergencyBeds) {
            maxIndex = left;
        }

        if (right < size &&
            heap[right]->emergencyBeds > heap[maxIndex]->emergencyBeds) {
            maxIndex = right;
        }

        if (index == maxIndex) break;

        Hospital* temp = heap[index];
        heap[index] = heap[maxIndex];
        heap[maxIndex] = temp;
        index = maxIndex;
    }
}

// ============ constructor / destructor ============

HospitalMaxHeap::HospitalMaxHeap(int cap) {
    capacity = (cap > 0 ? cap : 20);
    size = 0;
    heap = new Hospital * [capacity];
}

HospitalMaxHeap::~HospitalMaxHeap() {
    delete[] heap;  // only delete array, not hospital objects
}

// ============ public interface ============

void HospitalMaxHeap::insert(Hospital* h) {
    if (h == nullptr) {
        cout << "ERROR: Cannot insert null hospital pointer into heap.\n";
        return;
    }

    if (size >= capacity) {
        resize();
    }

    heap[size] = h;
    heapifyUp(size);
    size++;
}

Hospital* HospitalMaxHeap::extractMax() {
    if (size <= 0) {
        cout << "Heap is empty!" << endl;
        return nullptr;
    }

    Hospital* max = heap[0];
    heap[0] = heap[size - 1];
    size--;
    if (size > 0) {
        heapifyDown(0);
    }

    return max;
}

Hospital* HospitalMaxHeap::peek() {
    if (size > 0) return heap[0];
    return nullptr;
}

bool HospitalMaxHeap::isEmpty() {
    return size == 0;
}

void HospitalMaxHeap::rebuildHeap() {
    for (int i = size / 2 - 1; i >= 0; i--) {
        heapifyDown(i);
    }
}

void HospitalMaxHeap::display() {
    cout << "\n=== Emergency Bed Availability (Top Hospitals) ===" << endl;
    if (size == 0) {
        cout << "No hospitals in emergency bed heap.\n";
        return;
    }

    for (int i = 0; i < size; i++) {
        cout << i + 1 << ". " << heap[i]->name
            << " (" << heap[i]->hospitalID << ") - "
            << heap[i]->emergencyBeds << " beds" << endl;
        cout << "   Specialization: " << heap[i]->specialization << endl;
        cout << "   Doctors: " << heap[i]->doctorCount
            << " | Patients: " << heap[i]->patientCount << endl;
    }
}
void HospitalMaxHeap::displaySortedByBeds() {
    cout << "\n=== Emergency Bed Availability (Sorted by Beds) ===" << endl;
    if (size == 0) {
        cout << "No hospitals in emergency bed heap.\n";
        return;
    }

    // Create a temporary heap and copy all hospital pointers into it
    HospitalMaxHeap tempHeap(size);
    for (int i = 0; i < size; ++i) {
        tempHeap.insert(heap[i]);   
    }

    int rank = 1;
    while (!tempHeap.isEmpty()) {
        Hospital* h = tempHeap.extractMax();  // always returns current max
        cout << rank << ". " << h->name
            << " (" << h->hospitalID << ") - "
            << h->emergencyBeds << " beds" << endl;
        cout << "   Specialization: " << h->specialization << endl;
        cout << "   Doctors: " << h->doctorCount
            << " | Patients: " << h->patientCount << endl;
        ++rank;
    }
}

void HospitalMaxHeap::updateBedCount(string hospitalID, int newBedCount) {
    if (hospitalID.empty()) {
        cout << "Hospital ID cannot be empty.\n";
        return;
    }
    if (newBedCount < 0) {
        cout << "Invalid bed count. Must be >= 0.\n";
        return;
    }

    string targetID = toLower(hospitalID);

    for (int i = 0; i < size; i++) {
        if (toLower(heap[i]->hospitalID) == targetID) {
            int oldBeds = heap[i]->emergencyBeds;
            heap[i]->emergencyBeds = newBedCount;

            if (newBedCount > oldBeds) {
                heapifyUp(i);   // increased beds, move up
            }
            else if (newBedCount < oldBeds) {
                heapifyDown(i); // decreased beds, move down
            }

            cout << "Updated " << heap[i]->name
                << " beds: " << oldBeds << " -> " << newBedCount << endl;
            return;
        }
    }
    cout << "Hospital " << hospitalID << " not found in heap." << endl;
}
