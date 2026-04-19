#ifndef TRANSPORTSYSTEM_H
#define TRANSPORTSYSTEM_H

#include "Graph.h"
#include "Bus.h"
#include "HashTable.h"
#include "CircularQueue.h"
#include "Stack.h"

#include <string>
using namespace std;

// Simple hash map for stop queues
class StopQueueNode {
public:
	string stopID;
	CircularQueue* queue;
	StopQueueNode* next;

	StopQueueNode(string id, CircularQueue* q);
};

class TransportSystem {
private:
	Graph* cityGraph;
	// Graph* transportGraph;
	BusHashTable* busHash;
	bool ownsGraph;

	// Passenger Queue Management (Circular Queue)
	StopQueueNode* stopQueues;  // Linked list of queues for each stop
	int nextTicketNumber;

	bool parseStopsFile(string filename);
	bool parseBusesFile(string filename);
	string cleanString(string str);
	void parseCoordinates(string coordStr, double& lat, double& lon);
	void parseRoute(string routeStr, Bus* bus);
	double calculateDistance(double lat1, double lon1, double lat2, double lon2);
	//checker for school ID
	bool readValidSchoolID(Graph* cityGraph, string& outID);
	bool isValidString(string str);
	bool isValidCoordinate(double lat, double lon);


	// Helper for queue management
	CircularQueue* getOrCreateQueue(string stopID);
	string getCurrentTime();

public:
	TransportSystem(Graph* sharedGraph = nullptr);
	~TransportSystem();

	Graph* getCityGraph();

	void loadDataFromFiles(string stopsFile, string busesFile);

	void registerTransportCompany(string companyName);

	// Bus operations
	void registerBus(string busNo, string company, string currentStop);
	void addBusWithRoute(Bus* bus);
	Bus* getBusByNumber(string busNo);
	void updateBusLocation(string busNo, string newStop);
	void displayAllBuses();

	// Bus stop
	void addBusStop(string stopID, string name, double lat, double lon);
	void connectStops(string stop1, string stop2);
	void displayNetwork();

	// Find path (only through bus stops)
	void findShortestPath(string source, string destination);

	// Searching
	void searchNearestBus(double passengerLat, double passengerLon);
	string findNearestStop(double lat, double lon);

	// Passenger Queue Operations using Circular Queue
	void addPassengerToQueue(string stopID, string name, string destination);
	void boardPassengers(string busNo, string stopID, int count);
	void displayQueueAtStop(string stopID);
	void simulatePassengerArrival(string stopID);

	// Travel History Operations using Stack
	void recordBusStop(string busNo, string stopID);
	void displayBusHistory(string busNo);

	// Airport/Rail Integration
	void addAirportStop(string stopID, string name, double lat, double lon);
	void addRailwayStop(string stopID, string name, double lat, double lon);
	void findRouteToAirport(string fromStop);
	void displaySpecialStops();
	//school bus integration
	void registerSchoolBus(string schoolID, string busNo, string company);

	// Real-time Simulation
	void simulateBusMovement(string busNo, int delaySeconds);
	void runRealTimeSimulation();

	// Menu
	void displayMenu();
	void run();
};

#endif