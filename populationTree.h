#pragma once
#include "familyTree.h"
#include <iostream>
#include <string>
using namespace std;

struct houseNode {
	int houseNumber;
	personNode * familyHead;
	houseNode * next;
	int memberCount;

	double longitude;
	double latitude;

	houseNode(int hn, double lat = 0.0, double lon = 0.0) {
		houseNumber = hn;
		familyHead = nullptr;
		next = nullptr;
		memberCount = 0;
		longitude = lon;
		latitude = lat;
	}

		
	void setFamilyHead(personNode* head) {
		familyHead = head;
		
		memberCount = 1;
	}

	
	double getLatitude() const {
		return latitude;
	}

	double getLongitude() const {
		return longitude;
	}	

	void display() {
		cout << "\n   [HOUSE " << houseNumber << "] -----------------------" << endl;
		if (familyHead != nullptr) {
			cout << "   Family Head: " << familyHead->name << endl;
			cout << "   Members: " << memberCount << endl;

				
				familyHead->displayFamilyTree();
		}
		else {
			cout << "   (House is currently empty)" << endl;
		}
			cout << "   --------------------------------------" << endl;
		}
	};



struct streetNode {
	int streetNumber;
	streetNode* next;
	houseNode* houseHead;
	int houseCount;

	streetNode(int sn) {
		streetNumber = sn;
		houseHead = nullptr;
		houseCount = 0;
		next = nullptr;
	}

	~streetNode() {
		houseNode* temp = houseHead;
		while (temp != nullptr) {
			houseNode* toDelete = temp;
			temp = temp->next;
			delete toDelete;
		}
	}

	void addHouse(int houseNumber) {
		houseNode* newHouse = new houseNode(houseNumber);
		if (houseHead == nullptr) {
			houseHead = newHouse;
		}
		else {
			houseNode* temp = houseHead;
			while (temp->next != nullptr) {
				temp = temp->next;
			}
			temp->next = newHouse;
		}
		houseCount++;
	}

	void searchHouse(int houseNumber) {
		houseNode* temp = houseHead;
		while (temp != nullptr) {
			if (temp->houseNumber == houseNumber) {
				cout << "House " << houseNumber << " found." << endl;
				return;
			}
			temp = temp->next;
		}
		cout << "House " << houseNumber << " not found." << endl;
	}

	houseNode* searchHouseNode(int houseNumber) {
		houseNode* temp = houseHead;
		while (temp != nullptr) {
			if (temp->houseNumber == houseNumber) {
				return temp;
			}
			temp = temp->next;
		}
		return nullptr;
	}

	houseNode* getOrCreateHouse(int number) {
		houseNode* temp = searchHouseNode(number);
		if (temp != nullptr) return temp;

		addHouse(number);
		return searchHouseNode(number);
	}

	bool deleteHouse(int houseNumber) {
		houseNode* temp = houseHead;
		houseNode* prev = nullptr;
		while (temp != nullptr) {
			if (temp->houseNumber == houseNumber) {
				if (prev == nullptr) {
					houseHead = temp->next;
				}
				else {
					prev->next = temp->next;
				}
				delete temp;
				houseCount--;
				return true;
			}
			prev = temp;
			temp = temp->next;
		}
		return false;
	}


	void displayStreet() {
		
		cout << "\n    |=======================================|" << endl;
		cout << "    |            STREET " << streetNumber << "               |" << endl;
		cout << "    |=======================================|" << endl;

		houseNode* temp = houseHead;
		while (temp != nullptr) {
			
			temp->display();
			temp = temp->next;
		}

		cout << "    |_______________________________________|\n" << endl;
	}


};

struct sectorNode {
	string sectorName ;
	sectorNode * next;
	streetNode * streetHead;
	int streetCount;

	sectorNode( string name) {
		sectorName = name;
		streetHead = nullptr;
		next = nullptr;
		streetCount = 0;
	}

	~sectorNode() {
		streetNode* temp = streetHead;
		while (temp != nullptr) {
			streetNode* toDelete = temp;
			temp = temp->next;
			delete toDelete;
		}
	}

	void addStreet (int streetNumber) {
		streetNode * newStreet = new streetNode(streetNumber);
		if (streetHead == nullptr) {
			streetHead = newStreet;
		}
		else {
			streetNode * temp = streetHead;
			while (temp->next != nullptr) {
				temp = temp->next;
			}
			temp->next = newStreet;
		}
		streetCount++;
	}

	streetNode * searchStreetNode (int streetNumber) {
		streetNode * temp = streetHead;
		while (temp != nullptr) {
			if (temp->streetNumber == streetNumber) {
				return temp;
			}
			temp = temp->next;
		}
		return nullptr;
	}

	streetNode* getOrCreateStreet(int number) {
		streetNode* temp = searchStreetNode(number);
		if (temp != nullptr) return temp;

		addStreet(number);
		return searchStreetNode(number);
	}

	bool deleteStreet (int streetNumber) {
		streetNode * temp = streetHead;
		streetNode * prev = nullptr;
		while (temp != nullptr) {
			if (temp->streetNumber == streetNumber) {
				if (prev == nullptr) {
					streetHead = temp->next;
				}
				else {
					prev->next = temp->next;
				}
				delete temp;
				streetCount--;
				return true;
			}
			prev = temp;
			temp = temp->next;
		}
		return false;
	}

	void displaySector() {
		cout << "\n  ||=======================================||" << endl;
		cout << "  ||            SECTOR " << sectorName << "             ||" << endl;
		cout << "  ||=======================================||" << endl;

		streetNode * temp = streetHead;
		while (temp != nullptr) {
			temp->displayStreet();
			temp = temp->next;
		}

		cout << "  ||_______________________________________||\n" << endl;
	}

	
};
