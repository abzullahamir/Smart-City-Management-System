#include "Passenger.h"
#include <iostream>
using namespace std;

Passenger::Passenger() : name(""), destination(""), ticketNumber(0) {}

Passenger::Passenger(string passengerName, string dest, int ticket) : name(passengerName), destination(dest), ticketNumber(ticket) {}

string Passenger::getName() const {
    return name;
}

string Passenger::getDestination() const {
    return destination;
}

int Passenger::getTicketNumber() const {
    return ticketNumber;
}

void Passenger::setName(string passengerName) {
    name = passengerName;
}

void Passenger::setDestination(string dest) {
    destination = dest;
}

void Passenger::setTicketNumber(int ticket) {
    ticketNumber = ticket;
}

void Passenger::display() const {
    cout << "  Ticket #" << ticketNumber << " - " << name << " (to " << destination << ")" << endl;
}