#ifndef BUS_H
#define BUS_H

#include <string>
#include "LinkedList.h"
#include "Stack.h"
using namespace std;

// Represents a bus with route information
class Bus {
private:
	string busNo;
	string company;
	string currentStop;
	StringLinkedList* route; // Ordered list of Stop IDs
	Stack* travelHistory;    // Travel history stack

public:
	Bus();
	Bus(string number, string comp, string current);
	~Bus();

	// Getters
	string getBusNo() const;
	string getCompany() const;
	string getCurrentStop() const;
	StringLinkedList* getRoute() const;
	Stack* getTravelHistory() const;

	// Setters
	void setBusNo(string number);
	void setCompany(string comp);
	void setCurrentStop(string stopID);

	// Route management
	void addStopToRoute(string stopID);
	bool isStopInRoute(string stopID);

	// Travel history management
	void recordStopVisit(string stopID, string stopName, string timestamp);
	void displayTravelHistory() const;

	void display() const;
};


#endif