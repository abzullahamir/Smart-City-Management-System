#pragma once	

#include <iostream>
#include <string>

using namespace std;

struct personNode {
	string name;
	int age;
	string cnic;
	char gender; //male, female 
	string occupation;
	string relation; // father, mother, son ..

	personNode* spouse;
	personNode* next;
	personNode* childHead;

	personNode(string name, string cnic, string occupation, int age) {
		this->name = name;
		this->cnic = cnic;
		this->occupation = occupation;
		this->age = age;
		spouse = nullptr;
		next = nullptr;
		childHead = nullptr;
	}

	~personNode() {
		// Recursively delete children
		personNode* child = childHead;
		while (child != nullptr) {
			personNode* nextChild = child->next;
			delete child;
			child = nextChild;
		}


	}

	//first to enter the family tree will be considered as root / parent
	bool isParent() {
		return childHead != nullptr;
	}

	void displayRecursive(personNode* current, string prefix, bool isLast) {
		if (current == nullptr) return;

		// 1. Print the current person with tree branches
		cout << prefix;
		if (isLast) {
			cout << "|__ ";
			prefix += "    "; // Add spacing for children
		}
		else {
			cout << "|-- ";
			prefix += "|   "; // Add a vertical bar for siblings
		}


		cout << "[" << current->relation << "] " << current->name
			<< " (" << current->age << ", " << current->occupation << ")";

		if (current->spouse != nullptr) {
			cout << "  <==>  Spouse: " << current->spouse->name
				<< " (" << current->spouse->occupation << ")";
		}
		cout << endl;


		personNode* child = current->childHead;
		while (child != nullptr) {

			bool lastChild = (child->next == nullptr);


			displayRecursive(child, prefix, lastChild);


			child = child->next;
		}
	}

	void addSpouse(personNode* spouseNode) {
		this->spouse = spouseNode;
		spouseNode->relation = "Spouse"; 
		// We intentionally do NOT link back to avoid infinite recursion
	}

	void addChild(personNode* childNode) {
		if (childHead == nullptr) {
			childHead = childNode;
		}
		else {
			personNode* temp = childHead;
			while (temp->next != nullptr) {
				temp = temp->next;
			}
			temp->next = childNode;
		}
	}

	string extractFamilyCode() {
		if (cnic.length() > 13) 
			return cnic.substr(6, 7);
		return "";
	}

	void displayFamilyTree() {
		cout << "\n========================================" << endl;
		cout << " FAMILY TREE: " << name << endl;
		cout << "========================================" << endl;
		
		displayRecursive(this, "", true);

		cout << "========================================\n" << endl;
	}
};