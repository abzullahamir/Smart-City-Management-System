#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>
using namespace std;

// Represents a passenger waiting at a bus stop
class Passenger {
private:
    string name;
    string destination;
    int ticketNumber;

public:
    Passenger();
    Passenger(string passengerName, string dest, int ticket);

    // Getters
    string getName() const;
    string getDestination() const;
    int getTicketNumber() const;

    // Setters
    void setName(string passengerName);
    void setDestination(string dest);
    void setTicketNumber(int ticket);

    // Display
    void display() const;
};

#endif