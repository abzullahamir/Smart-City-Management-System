#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include "Node.h"
using namespace std;

// Edge in adjacency list
class Edge {
public:
	string destNodeID;
	double weight; // Distance in km
	Edge* next;

	Edge(string dest, double w);
};

// Graph node containing stops and its edges
class GraphNode {
public:
	Node node;
	Edge* edges;
	GraphNode* next;

	GraphNode(Node s);
	~GraphNode();
};

// Graph using adjacency list
class Graph {
private:
	GraphNode* head;
	int vertexCount;

	double calculateDistance(double lat1, double lon1, double lat2, double lon2);

public:
	Graph();
	~Graph();

	void addNode(Node stop);
	void addEdge(string src, string dest, double weight);
	bool hasNode(string stopID);
	Node* getNode(string stopID);
	void displayGraph(string nodeTypeFilter = "ANY");
	GraphNode* getHead() const { return head; }
	GraphNode* findNode(string stopID);


	// Shortest path algorithm
	void dijkstraAlgo(string source, string destination, string nodeTypeFilter);

	// Find nearest node of specific type
	string findNearestNode(double lat, double lon, string nodeTypeFilter);

	// Connect node to nearby nodes within radius
	void connectToNearbyNodes(string nodeID, double radiusKm, string targetType);

	void displayNodesByType(string nodeType);

	void exportNodesCSV(const string& filename = "nodes.csv");
	void exportEdgesCSV(const string& filename = "edges.csv");

	int getVertexCount() const;

	//for school bus tracking
	int getNearByHouses(string schoolID, string houseIDs[], int maxHouses, double radiusKm);
};

#endif