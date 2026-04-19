#ifndef NODE_H
#define NODE_H

#include <string>
using namespace std;

// -Bus stops(type: "BUS_STOP")
// -Malls(type: "MALL")
// -Airports(type: "AIRPORT")
// -Railway stations(type: "RAILWAY")
// -Schools(type: "SCHOOL")
// -Hospitals(type: "HOSPITAL")

// Graph vertex
class Node {
private:
	string nodeID;
	string name;
	string nodeType;
	double latitude;
	double longitude;

public:
	Node();
	Node(string id, string name, double lat, double lon);
	Node(string id, string name, string type, double lat, double lon);

	// Getters
	string getNodeID() const;
	string getName() const;
	string getNodeType() const;
	double getLatitude() const;
	double getLongitude() const;

	// Setters
	void setNodeID(string id);
	void setName(string name);
	void setNodeType(string type);
	void setCoordinates(double lat, double lon);

	void display() const;
};

#endif