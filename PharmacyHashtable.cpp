#include "pharmacyHashtable.h"

using namespace std;

// ============ helper functions ============

int PharmacyHashTable::stringLen(const string& str) const {
    return static_cast<int>(str.length());
}

string PharmacyHashTable::toLower(const string& str) const {
    string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = static_cast<char>(result[i] + 32);
        }
    }
    return result;
}

int PharmacyHashTable::hash(const string& key) const {
    // case-insensitive hashing
    string lowerKey = toLower(key);
    unsigned long h = 0;
    int prime_base = 31;

    for (int i = 0; i < stringLen(lowerKey); i++) {
        h = (h * prime_base + static_cast<int>(lowerKey[i])) % tableSize;
    }
    return static_cast<int>(h);
}

Medicine* PharmacyHashTable::searchInTable(MedicineNode** table, const string& key) {
    if (key.empty() || tableSize <= 0) return nullptr;

    int idx = hash(key);
    MedicineNode* node = table[idx];
    string lowerKey = toLower(key);

    while (node != nullptr) {
        if (toLower(node->key) == lowerKey) {
            return &node->value;
        }
        node = node->next;
    }
    return nullptr;
}

bool PharmacyHashTable::removeFromTable(MedicineNode** table, const string& key) {
    if (key.empty() || tableSize <= 0) return false;

    int idx = hash(key);
    MedicineNode* node = table[idx];
    MedicineNode* prev = nullptr;
    string lowerKey = toLower(key);

    while (node != nullptr) {
        if (toLower(node->key) == lowerKey) {
            if (prev == nullptr) {
                table[idx] = node->next;
            }
            else {
                prev->next = node->next;
            }
            delete node;
            return true;
        }
        prev = node;
        node = node->next;
    }
    return false;
}

// ============ constructor / destructor ============

PharmacyHashTable::PharmacyHashTable(int size) {
    tableSize = (size > 0 ? size : 20);

    // name table
    nameTable = new MedicineNode * [tableSize];
    for (int i = 0; i < tableSize; i++) {
        nameTable[i] = nullptr;
    }

    // formula table
    formulaTable = new MedicineNode * [tableSize];
    for (int i = 0; i < tableSize; i++) {
        formulaTable[i] = nullptr;
    }
}

PharmacyHashTable::~PharmacyHashTable() {
    // delete name table
    for (int i = 0; i < tableSize; i++) {
        MedicineNode* entry = nameTable[i];
        while (entry != nullptr) {
            MedicineNode* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] nameTable;

    // delete formula table
    for (int i = 0; i < tableSize; i++) {
        MedicineNode* entry = formulaTable[i];
        while (entry != nullptr) {
            MedicineNode* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] formulaTable;
}

// ============ public interface ============

void PharmacyHashTable::insert(string medicineName, Medicine value) {
    if (medicineName.empty()) {
        cout << "Medicine name cannot be empty.\n";
        return;
    }

    // insert/update in name table
    {
        int idx = hash(medicineName);
        MedicineNode* node = nameTable[idx];

        while (node != nullptr) {
            if (toLower(node->key) == toLower(medicineName)) {
                node->value = value;
                cout << "Updated: " << medicineName
                    << " (" << value.formula << ")\n";
                // also update corresponding entry in formulaTable
                // so price/formula stay in sync
                Medicine* fMed = search_by_formula(value.formula);
                if (fMed != nullptr) {
                    *fMed = value;
                }
                return;
            }
            node = node->next;
        }

        MedicineNode* newNode = new MedicineNode(medicineName, value);
        newNode->next = nameTable[idx];
        nameTable[idx] = newNode;
    }

    // insert/update in formula table
    {
        string formulaKey = value.formula;
        int idx = hash(formulaKey);
        MedicineNode* node = formulaTable[idx];

        while (node != nullptr) {
            if (toLower(node->key) == toLower(formulaKey)) {
                node->value = value;
                cout << "Updated: " << medicineName
                    << " (" << value.formula << ")\n";
                return;
            }
            node = node->next;
        }

        MedicineNode* newNode = new MedicineNode(formulaKey, value);
        newNode->next = formulaTable[idx];
        formulaTable[idx] = newNode;
    }

    cout << "Added: " << medicineName << " (" << value.formula << ")" << endl;
}

Medicine* PharmacyHashTable::search(string medicine_name) {
    return searchInTable(nameTable, medicine_name);
}

Medicine* PharmacyHashTable::search_by_formula(string formula) {
    return searchInTable(formulaTable, formula);
}

bool PharmacyHashTable::remove(string key) {
    if (key.empty()) return false;

    // find the medicine by name first
    Medicine* med = search(key);
    if (med == nullptr) {
        return false;  // not found
    }

    string formula = med->formula;

    // remove from both tables (case-insensitive)
    bool removedName = removeFromTable(nameTable, key);
    bool removedFormula = removeFromTable(formulaTable, formula);

    if (removedName || removedFormula) {
        cout << "Removed medicine " << key << " from pharmacy inventory.\n";
        return true;
    }
    return false;
}

bool PharmacyHashTable::removeByFormula(string formula) {
    if (formula.empty()) return false;

    // find the medicine by formula first
    Medicine* med = search_by_formula(formula);
    if (med == nullptr) {
        return false;  // not found
    }

    string name = med->medicineName;

    bool removedFormula = removeFromTable(formulaTable, formula);
    bool removedName = removeFromTable(nameTable, name);

    if (removedName || removedFormula) {
        cout << "Removed medicine with formula " << formula
            << " from pharmacy inventory.\n";
        return true;
    }
    return false;
}

void PharmacyHashTable::display() {
    cout << "\n=== Pharmacy Medicines ===" << endl;
    if (tableSize <= 0) {
        cout << "No medicines available.\n";
        return;
    }

    bool any = false;
    for (int i = 0; i < tableSize; i++) {
        if (nameTable[i] != nullptr) {
            MedicineNode* node = nameTable[i];
            while (node != nullptr) {
                any = true;
                cout << "  " << node->value.medicineName
                    << " (" << node->value.formula << ") - Rs."
                    << node->value.price << endl;
                node = node->next;
            }
        }
    }

    if (!any) {
        cout << "No medicines available.\n";
    }
}
