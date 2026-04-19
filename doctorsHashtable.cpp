#include "doctorsHashtable.h"

int DoctorHashTable::stringLen(const string& str) const {
    return str.length();
}

string DoctorHashTable::toLower(const string& str) const {
    string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] += 32;
        }
    }
    return result;
}

int DoctorHashTable::hash(const string& key) const {
    string lowerKey = toLower(key);
    unsigned long h = 0;
    int primeBase = 31;

    for (int i = 0; i < stringLen(lowerKey); i++) {
        h = (h * primeBase + static_cast<int>(lowerKey[i])) % tableSize;
    }
    return static_cast<int>(h);
}

DoctorHashTable::DoctorHashTable(int size) : tableSize(size > 0 ? size : 20) {
    table = new DoctorHashNode * [tableSize];
    for (int i = 0; i < tableSize; i++) {
        table[i] = nullptr;
    }
    cout << "Doctor Hash Table created with size " << tableSize << "\n";
}

DoctorHashTable::~DoctorHashTable() {
    for (int i = 0; i < tableSize; i++) {
        DoctorHashNode* entry = table[i];
        while (entry != nullptr) {
            DoctorHashNode* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] table;
}

bool DoctorHashTable::insert(const string& specialization, Doctor* doctorPtr, Hospital* hospitalPtr) {
    if (doctorPtr == nullptr || hospitalPtr == nullptr) {
        cout << "ERROR: Cannot insert null doctor or hospital pointer.\n";
        return false;
    }

    int idx = hash(specialization);
    DoctorHashNode* newNode = new DoctorHashNode(specialization, doctorPtr, hospitalPtr);
    newNode->next = table[idx];
    table[idx] = newNode;

    cout << "✓ Indexed Dr. " << doctorPtr->name << " (" << specialization
        << ") at " << hospitalPtr->name << "\n";
    return true;
}

void DoctorHashTable::searchBySpecialization(const string& specialization) const {
    cout << "\n--- Doctors specializing in: " << specialization << " ---\n";

    int idx = hash(specialization);
    if (table[idx] == nullptr) {
        cout << "No doctors found with specialization: " << specialization << "\n";
        return;
    }

    DoctorHashNode* node = table[idx];
    int count = 0;
    string lowerSpec = toLower(specialization);

    while (node != nullptr) {
        if (toLower(node->key) == lowerSpec && node->doctor != nullptr && node->hospital != nullptr) {
            count++;
            cout << count << ". Dr. " << node->doctor->name << " - "
                << node->hospital->name << " (" << node->hospital->sector << ")\n";
        }
        node = node->next;
    }

    if (count == 0) {
        cout << "No active doctors found with specialization: " << specialization << "\n";
    }
    else {
        cout << "Total: " << count << " doctor(s) found.\n";
    }
}

bool DoctorHashTable::removeDoctor(const string& doctorName, const string& hospitalID) {
    string lowerDocName = toLower(doctorName);
    string lowerHospitalID = toLower(hospitalID);

    for (int i = 0; i < tableSize; i++) {
        DoctorHashNode* node = table[i];
        DoctorHashNode* prev = nullptr;

        while (node != nullptr) {
            if (toLower(node->doctor->name) == lowerDocName &&
                toLower(node->hospital->hospitalID) == lowerHospitalID) {

                // unlink
                if (prev == nullptr) {
                    table[i] = node->next;
                }
                else {
                    prev->next = node->next;
                }

                delete node;
                cout << "✓ Removed Dr. " << doctorName << " from hash table.\n";
                return true;
            }

            prev = node;
            node = node->next;
        }
    }
    return false;
}


bool DoctorHashTable::doctorExists(const string& doctorName, const string& hospitalID) const {
    string lowerDocName = toLower(doctorName);
    string lowerHospitalID = toLower(hospitalID);

    for (int i = 0; i < tableSize; i++) {
        DoctorHashNode* node = table[i];
        while (node != nullptr) {
            if (toLower(node->doctor->name) == lowerDocName &&
                toLower(node->hospital->hospitalID) == lowerHospitalID) {
                return true;
            }
            node = node->next;
        }
    }
    return false;
}

void DoctorHashTable::displayClean() const {
    cout << "\n========== DOCTORS BY SPECIALIZATION ==========\n";
    int totalDoctors = 0;

    for (int i = 0; i < tableSize; i++) {
        DoctorHashNode* node = table[i];
        while (node != nullptr) {
            if (node->doctor != nullptr && node->hospital != nullptr) {
                totalDoctors++;
                cout << totalDoctors << ". Dr. " << node->doctor->name
                    << " (" << node->key << ") - " << node->hospital->name
                    << " (" << node->hospital->sector << ")\n";
            }
            node = node->next;
        }
    }

    cout << "Total Doctors: " << totalDoctors << "\n";
    cout << "==============================================\n";
}

int DoctorHashTable::getDoctorsBySpecialization(const string& specialization,
    Doctor** outDoctors,
    Hospital** outHospitals,
    int maxResults) const {
    int idx = hash(specialization);
    int count = 0;
    string lowerSpec = toLower(specialization);

    if (table[idx] == nullptr) {
        return 0;
    }

    DoctorHashNode* node = table[idx];
    while (node != nullptr && count < maxResults) {
        if (toLower(node->key) == lowerSpec &&
            node->doctor != nullptr &&
            node->hospital != nullptr) {
            outDoctors[count] = node->doctor;
            outHospitals[count] = node->hospital;
            count++;
        }
        node = node->next;
    }
    return count;
}
