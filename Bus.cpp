#include "Bus.h"

#include <iostream>
using namespace std;

Bus::Bus() : busNo(""), company(""), currentStop("") {
	route = new StringLinkedList();
    travelHistory = new Stack(50);  // Store last 50 stops
}

Bus::Bus(string number, string comp, string current) : busNo(number), company(comp), currentStop(current) {
	route = new StringLinkedList();
    travelHistory = new Stack(50);
}

Bus::~Bus() {
	delete route;
    delete travelHistory;
}

string Bus::getBusNo() const {
	return busNo;
}

string Bus::getCompany() const {
	return company;
}

string Bus::getCurrentStop() const {
	return currentStop;
}

StringLinkedList* Bus::getRoute() const {
	return route;
}

void Bus::setBusNo(string number) {
	busNo = number;
}

void Bus::setCompany(string comp) {
    company = comp;
}

void Bus::setCurrentStop(string stopID) {
    currentStop = stopID;
}

void Bus::addStopToRoute(string stopID) {
    route->append(stopID);
}

bool Bus::isStopInRoute(string stopID) {
    return route->contains(stopID);
}

void Bus::recordStopVisit(string stopID, string stopName, string timestamp) {
    travelHistory->push(stopID, stopName, timestamp);
}

void Bus::displayTravelHistory() const {
    cout << endl << "===== Travel History for Bus " << busNo << " =====" << endl;
    travelHistory->display();
}

void Bus::display() const {
    cout << endl << "----- Bus Details -----" << endl;
    cout << "Bus No: " << busNo << endl;
    cout << "Company: " << company << endl;
    cout << "Current Stop: " << currentStop << endl;
    cout << "Route: ";
    route->display();
}
