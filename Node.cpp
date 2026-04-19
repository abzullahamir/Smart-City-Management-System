#include "Node.h"

#include <iostream>
using namespace std;

Node::Node() : nodeID(""), name(""), latitude(0.0), longitude(0.0) {}

Node::Node(string id, string name, double lat, double lon) : nodeID(id), name(name), latitude(lat), longitude(lon) {}

Node::Node(string id, string stopName, string type, double lat, double lon) : nodeID(id), name(stopName), nodeType(type), latitude(lat), longitude(lon) {}

string Node::getNodeID() const {
	return nodeID;
}

string Node::getName() const {
	return name;
}

string Node::getNodeType() const {
	return nodeType;
}

double Node::getLatitude() const {
	return latitude;
}

double Node::getLongitude() const {
	return longitude;
}

void Node::setNodeID(string id) {
	nodeID = id;
}

void Node::setName(string name) {
	this->name = name;
}

void Node::setNodeType(string type) {
    nodeType = type;
}

void Node::setCoordinates(double lat, double lon) {
	latitude = lat;
	longitude = lon;
}

void Node::display() const {
	cout << "Stop ID: " << nodeID << endl;
	cout << "Name: " << name << endl;
	cout << "Type: " << nodeType << endl;
	cout << "Coordinates: (" << latitude << ", " << longitude << ")" << endl;
}

