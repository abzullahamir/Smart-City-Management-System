#include "hospitalHashTable.h"

bool HospitalHashTable::stringsEqualCaseInsensitive(const string& s1, const string& s2) const {
    if (s1.length() != s2.length()) return false;
    string lower1 = toLowerCase(s1);
    string lower2 = toLowerCase(s2);
    return lower1 == lower2;
}

string HospitalHashTable::toLowerCase(const string& str) const {
    string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] += 32;
        }
    }
    return result;
}

int HospitalHashTable::stringLen(const string& str) const {
    return str.length();
}

int HospitalHashTable::hash(const string& key) const {
    unsigned long h = 0;
    int primeBase = 31;
    string lowerKey = toLowerCase(key);

    for (int i = 0; i < stringLen(lowerKey); i++) {
        h = (h * primeBase + static_cast<int>(lowerKey[i])) % tableSize;
    }
    return static_cast<int>(h);
}

HospitalHashTable::HospitalHashTable(int size) : tableSize(size > 0 ? size : 20) {
    table = new HospitalHashNode * [tableSize];
    for (int i = 0; i < tableSize; i++) {
        table[i] = nullptr;
    }
    cout << "Hospital Hash Table created with size " << tableSize << "\n";
}

HospitalHashTable::~HospitalHashTable() {
    for (int i = 0; i < tableSize; i++) {
        HospitalHashNode* entry = table[i];
        while (entry != nullptr) {
            HospitalHashNode* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] table;
}

bool HospitalHashTable::insert(const string& key, Hospital* hospitalPtr) {
    if (hospitalPtr == nullptr) {
        cout << "ERROR: Cannot insert null hospital pointer.\n";
        return false;
    }

    if (exists(key)) {
        cout << "ERROR: Hospital ID '" << key << "' already exists! Duplicate prevented.\n";
        return false;
    }

    int idx = hash(key);
    HospitalHashNode* newNode = new HospitalHashNode(key, hospitalPtr);
    newNode->next = table[idx];
    table[idx] = newNode;

    cout << "✓ Hospital '" << key << "' indexed successfully.\n";
    return true;
}

Hospital* HospitalHashTable::search(const string& key) const {
    int idx = hash(key);
    HospitalHashNode* node = table[idx];

    while (node != nullptr) {
        if (stringsEqualCaseInsensitive(node->key, key)) {
            return node->value;
        }
        node = node->next;
    }
    return nullptr;
}

bool HospitalHashTable::remove(const string& key) {
    int idx = hash(key);
    if (table[idx] == nullptr) {
        return false;
    }

    HospitalHashNode* node = table[idx];
    if (stringsEqualCaseInsensitive(node->key, key)) {
        table[idx] = node->next;
        delete node;
        cout << "✓ Removed hospital '" << key << "' from hash table.\n";
        return true;
    }

    while (node->next != nullptr) {
        if (stringsEqualCaseInsensitive(node->next->key, key)) {
            HospitalHashNode* temp = node->next;
            node->next = node->next->next;
            delete temp;
            cout << "✓ Removed hospital '" << key << "' from hash table.\n";
            return true;
        }
        node = node->next;
    }
    return false;
}

bool HospitalHashTable::exists(const string& key) const {
    return search(key) != nullptr;
}

int HospitalHashTable::getTotalCount() const {
    int count = 0;
    for (int i = 0; i < tableSize; i++) {
        HospitalHashNode* node = table[i];
        while (node != nullptr) {
            count++;
            node = node->next;
        }
    }
    return count;
}

void HospitalHashTable::displayClean() const {
    cout << "\n========== ALL HOSPITALS (Unified View) ==========\n";

    int count = 0;
    for (int i = 0; i < tableSize; i++) {
        HospitalHashNode* node = table[i];
        while (node != nullptr) {
            count++;
            cout << count << ". ";
            cout << node->key << " - " << node->value->name << "\n";
            cout << "   Sector: " << node->value->sector << "\n";
            cout << "   Beds: " << node->value->emergencyBeds << "\n";
            cout << "   Specialization: " << node->value->specialization << "\n";
            cout << "   Location: (" << node->value->latitude << ", " << node->value->longitude << ")\n";
            cout << "----------------------------------------\n";
            node = node->next;
        }
    }

    cout << "Total Hospitals: " << count << "\n";
    cout << "==============================================\n";
}

void HospitalHashTable::displayHospitalByID(const string& hospitalID) const {
    Hospital* h = search(hospitalID);
    if (h != nullptr) {
        cout << "\n--- " << h->name << " (ID: " << h->hospitalID << ") ---\n";
        cout << "Sector: " << h->sector << "\n";
        cout << "Emergency Beds: " << h->emergencyBeds << "\n";
        cout << "Specialization: " << h->specialization << "\n";
        cout << "Location: (" << h->latitude << ", " << h->longitude << ")\n";
        cout << "----------------------------------------\n";
    }
    else {
        cout << "Hospital '" << hospitalID << "' not found.\n";
    }
}

int HospitalHashTable::getAllHospitals(Hospital** results, int maxResults) const {
    int count = 0;
    for (int i = 0; i < tableSize && count < maxResults; i++) {
        HospitalHashNode* curr = table[i];
        while (curr != nullptr && count < maxResults) {
            results[count++] = curr->value;
            curr = curr->next;
        }
    }
    return count;
}
