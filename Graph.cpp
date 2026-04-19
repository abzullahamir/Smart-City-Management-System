#include "Graph.h"
#include "MinHeap.h"

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

const double INFINITE = DBL_MAX; // max value in double
const double PI = 3.14159265358979323846;
const double EARTH_RADIUS = 6371.0; // km

Edge::Edge(string dest, double w) : destNodeID(dest), weight(w), next(nullptr) {}

GraphNode::GraphNode(Node n) : node(n), edges(nullptr), next(nullptr) {}

GraphNode::~GraphNode() {
    Edge* current = edges;
    while (current) {
        Edge* temp = current;
        current = current->next;
        delete temp;
    }
}

Graph::Graph() : head(nullptr), vertexCount(0) {}

Graph::~Graph() {
    GraphNode* current = head;
    while (current) {
        GraphNode* temp = current;
        current = current->next;
        delete temp;
    }
}

GraphNode* Graph::findNode(string stopID) {
    GraphNode* current = head;
    while (current) {
        if (current->node.getNodeID() == stopID) {
            return current;
        }
        current = current->next;
    }

    return nullptr;
}

void Graph::addNode(Node stop) {
    if (hasNode(stop.getNodeID())) {
        cout << "Stop already exists!" << endl;
        return;
    }

    GraphNode* newNode = new GraphNode(stop);
    newNode->next = head;
    head = newNode;
    vertexCount++;
}

void Graph::addEdge(string src, string dest, double weight) {
    if (weight < 0) {
        cout << "Warning: Negative edge weight! Setting to 0." << endl;
        weight = 0;
    }

    if (weight > 5000) {  // More than 1000km seems wrong
        cout << "Warning: Very large distance (" << weight << " km). Please verify." << endl;
    }

    GraphNode* srcNode = findNode(src);
    if (!srcNode) {
        cout << "Source stop not found!" << endl;
        return;
    }

    if (!findNode(dest)) {
        cout << "Destination stop not found!" << endl;
        return;
    }

    // Add edge from source to destination
    Edge* newEdge = new Edge(dest, weight);
    newEdge->next = srcNode->edges;
    srcNode->edges = newEdge;
}

bool Graph::hasNode(string stopID) {
    if (findNode(stopID)) {
        return true;
    }
    else {
        return false;
    }
}

Node* Graph::getNode(string stopID) {
    GraphNode* node = findNode(stopID);
    if (node) {
        return &(node->node);
    }
    else {
        return nullptr;
    }
}

// Haversine formula for calculating distance
double Graph::calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Convert degree difference into radians
    double diffLat = (lat2 - lat1) * PI / 180.0;
    double diffLon = (lon2 - lon1) * PI / 180.0;

    // Convert latitude to radians
    lat1 = lat1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;

    // Haversine formula
    double a = sin(diffLat / 2) * sin(diffLat / 2) + sin(diffLon / 2) * sin(diffLon / 2) * cos(lat1) * cos(lat2);

    if (a > 1.0) a = 1.0;
    if (a < 0.0) a = 0.0;

    // Convert to angle (C = 2 * arcsin( sqrt(a) )), but safer version
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    // Convert angle to distance (S = r * theta)
    return EARTH_RADIUS * c;
}

string Graph::findNearestNode(double lat, double lon, string nodeTypeFilter) {
    if (!head) return "";

    GraphNode* current = head;
    string nearestNodeID = "";
    double minDistance = INFINITE;

    // Find first node of matching type
    while (current) {
        if (nodeTypeFilter == "ANY" || current->node.getNodeType() == nodeTypeFilter) {
            nearestNodeID = current->node.getNodeID();
            minDistance = calculateDistance(lat, lon, current->node.getLatitude(), current->node.getLongitude());
            break;
        }
        current = current->next;
    }

    if (nearestNodeID == "") return "";

    // Continue searching for closer nodes
    current = current->next;
    while (current) {
        if (nodeTypeFilter == "ANY" || current->node.getNodeType() == nodeTypeFilter) {
            double distance = calculateDistance(lat, lon, current->node.getLatitude(), current->node.getLongitude());
            if (distance < minDistance) {
                minDistance = distance;
                nearestNodeID = current->node.getNodeID();
            }
        }
        current = current->next;
    }

    return nearestNodeID;
}


void Graph::dijkstraAlgo(string source, string destination, string nodeTypeFilter) {
    if (!hasNode(source) || !hasNode(destination)) {
        cout << "Source or destination stop not found!" << endl;
        return;
    }

    // Initialize
    const int MAX_NODES = 1000;
    string* nodeIDs = new string[MAX_NODES];
    double* distances = new double[MAX_NODES];
    string* parent = new string[MAX_NODES];
    bool* visited = new bool[MAX_NODES];

    int index = 0;
    GraphNode* current = head;
    while (current) {
        nodeIDs[index] = current->node.getNodeID();
        distances[index] = INFINITE;
        parent[index] = "";
        visited[index] = false;

        if (nodeIDs[index] == source) {
            distances[index] = 0; // Starting node
        }

        index++;
        current = current->next;
    }

    int totalStops = index;

    // Priority Queue using MinHeap
    MinHeap pQueue(MAX_NODES);
    pQueue.insert(source, 0);

    while (!pQueue.isEmpty()) {
        HeapNode minNode = pQueue.extractMin();
        string currentStop = minNode.stopID;

        // Find index of current stop
        int currentIndex = -1;
        for (int i = 0; i < totalStops; i++) {
            if (nodeIDs[i] == currentStop) {
                currentIndex = i;
                break;
            }
        }

        if (currentIndex == -1 || visited[currentIndex]) {
            continue;
        }

        visited[currentIndex] = true;

        if (currentStop == destination) {
            break;
        }

        // Get edges of current stop
        GraphNode* node = findNode(currentStop);
        Edge* edge = node->edges;

        while (edge) {
            // Find adjacent nodes (neighbours)
            GraphNode* neighborNode = findNode(edge->destNodeID);

            if (nodeTypeFilter != "ANY" && edge->destNodeID != destination && neighborNode->node.getNodeType() != nodeTypeFilter) {
                edge = edge->next;
                continue;
            }

            int neighbourIndex = -1;
            for (int i = 0; i < totalStops; i++) {
                if (nodeIDs[i] == edge->destNodeID) {
                    neighbourIndex = i;
                    break;
                }
            }

            if (neighbourIndex != -1 && !visited[neighbourIndex]) {
                double newDistance = distances[currentIndex] + edge->weight;

                if (newDistance < distances[neighbourIndex]) {
                    distances[neighbourIndex] = newDistance;
                    parent[neighbourIndex] = currentStop;
                    pQueue.insert(edge->destNodeID, newDistance);
                }
            }

            edge = edge->next;
        }
    }

    // Print Path
    int destinationIndex = -1;
    for (int i = 0; i < totalStops; i++) {
        if (nodeIDs[i] == destination) {
            destinationIndex = i;
            break;
        }
    }

    if (destinationIndex == -1 || distances[destinationIndex] == INFINITE) {
        cout << "No path found!" << endl;
    }
    else {
        cout << endl << "Shortest path from " << source << " to " << destination << ":" << endl;
        cout << "Total distance: " << distances[destinationIndex] << " km" << endl;


        // Constuct the path in reverse
        string path[MAX_NODES];
        int pathLength = 0;
        string curr = destination;
        
        while (curr != "") {
            path[pathLength++] = curr;
            for (int i = 0; i < totalStops; i++) {
                if (nodeIDs[i] == curr) {
                    curr = parent[i];
                    break;
                }
            }
        }

        cout << "Path: ";
        for (int i = pathLength - 1; i >= 0; i--) {
            cout << path[i];
            if (i > 0) {
                cout << " -> ";
            }
        }
        
        cout << endl;
    }

    delete[] nodeIDs;
    delete[] distances;
    delete[] parent;
    delete[] visited;
}

void Graph::connectToNearbyNodes(string nodeID, double radiusKm, string targetType) {
    GraphNode* node = findNode(nodeID);
    if (!node) {
        cout << "Node not found: " << nodeID << endl;
        return;
    }

    double nodeLat = node->node.getLatitude();
    double nodeLon = node->node.getLongitude();

    GraphNode* current = head;
    int connectionsAdded = 0;

    while (current) {
        // Don't connect to self
        if (current->node.getNodeID() == nodeID) {
            current = current->next;
            continue;
        }

        // Check if target type matches
        if (targetType != "ANY" && current->node.getNodeType() != targetType) {
            current = current->next;
            continue;
        }

        // Calculate distance
        double distance = calculateDistance(nodeLat, nodeLon, current->node.getLatitude(), current->node.getLongitude());

        // If within radius, create bidirectional edge
        if (distance <= radiusKm && distance > 0) {
            addEdge(nodeID, current->node.getNodeID(), distance);
            addEdge(current->node.getNodeID(), nodeID, distance);
            connectionsAdded++;
        }

        current = current->next;
    }

    cout << "Connected " << nodeID << " to " << connectionsAdded << " nearby nodes.\n";
}

void Graph::displayGraph(string nodeTypeFilter) {
    if (nodeTypeFilter == "ANY") {
        cout << endl << "========== City Graph (All Nodes) ==========" << endl;
    }

    if (!head) {
        cout << "Graph is empty." << endl;
        return;
    }

    GraphNode* current = head;


    int busStops = 0, malls = 0, airports = 0, railways = 0;
    int schools = 0, houses = 0, others = 0;
    int hospitals = 0, pharmacies = 0;
    int mosques = 0, parks = 0, waterCoolers = 0;

    GraphNode* temp = head;
    while (temp) {
        if (nodeTypeFilter == "ANY" || temp->node.getNodeType() == nodeTypeFilter) {
            string type = temp->node.getNodeType();

            if (type == "BUS_STOP") busStops++;
            else if (type == "MALL") malls++;
            else if (type == "AIRPORT") airports++;
            else if (type == "RAILWAY") railways++;
            else if (type == "SCHOOL") schools++; // Count Schools
            else if (type == "HOUSE") houses++;   //Count Houses (Population Integration)
            else if (type == "HOSPITAL") hospitals++;
            else if (type == "PHARMACY") pharmacies++;
            else if (type == "MOSQUE") mosques++;
            else if (type == "PARK") parks++;
            else if (type == "WATER_COOLER") waterCoolers++;
            else others++;
        }
        temp = temp->next;
    }

    cout << "Total Nodes: " << vertexCount << endl;

    // Display counts based on filter
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "BUS_STOP")
        cout << "   Bus Stops: " << busStops << endl;
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "MALL")
        cout << "   Malls: " << malls << endl;
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "AIRPORT")
        cout << "   Airports: " << airports << endl;
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "RAILWAY")
        cout << "   Railways: " << railways << endl;


    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "SCHOOL")
        cout << "   Schools: " << schools << endl;
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "HOUSE")
        cout << "   Houses: " << houses << endl;
    // -------------------------
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "HOSPITAL")
        cout << "   Hospitals: " << hospitals << endl;
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "PHARMACY")
        cout << "   Pharmacies: " << pharmacies << endl;
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "MOSQUE")
        cout << "   Mosques: " << mosques << endl;
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "PARK")
        cout << "   Parks: " << parks << endl;
    if (nodeTypeFilter == "ANY" || nodeTypeFilter == "WATER_COOLER")
        cout << "   Water Coolers: " << waterCoolers << endl;

    if (others > 0 && (nodeTypeFilter == "ANY"))
        cout << "   Others: " << others << endl;

    cout << endl;

    while (current) {
        if (nodeTypeFilter == "ANY" || current->node.getNodeType() == nodeTypeFilter) {
            cout << current->node.getNodeID() << " [" << current->node.getNodeType() << "] (" << current->node.getName() << ")";

            Edge* edge = current->edges;

            if (edge) {
                cout << " -> ";

                int edgeCount = 0;
                while (edge && edgeCount < 5) {
                    cout << edge->destNodeID << " (" << edge->weight << "km)";

                    if (edge->next && edgeCount < 4) {
                        cout << ", ";
                    }

                    edge = edge->next;
                    edgeCount++;
                }

                if (edge) cout << "...";
            }
            cout << endl;
        }
        current = current->next;
    }
}

int Graph::getVertexCount() const {
    return vertexCount;
}

//for school bus tracking

int Graph::getNearByHouses(string schoolID, string houseIDs[], int maxHouses, double radiusKm) {
    GraphNode* schoolNode = findNode(schoolID);
    if (!schoolNode) {
        cout << "School node not found: " << schoolID << endl;
        return 0;
    }

    double schoolLat = schoolNode->node.getLatitude();
    double schoolLon = schoolNode->node.getLongitude();

    GraphNode* current = head;
    int housesFound = 0;

    while (current && housesFound < maxHouses) {

        if (current->node.getNodeID() == schoolID) {
            current = current->next;
            continue;
        }


        if (current->node.getNodeType() == "HOUSE") {

            double distance = calculateDistance(schoolLat, schoolLon, current->node.getLatitude(), current->node.getLongitude());


            if (distance <= radiusKm) {
                houseIDs[housesFound] = current->node.getNodeID();
                housesFound++;
            }
        }

        current = current->next;
    }

    return housesFound;
}

void Graph::displayNodesByType(string nodeType) {
    GraphNode* current = head;
    int count = 0;

    cout << endl << "===== Nodes of Type: " << nodeType << " =====" << endl;

    while (current) {
        if (current->node.getNodeType() == nodeType) {
            count++;
            cout << count << ". " << current->node.getNodeID() << " - " << current->node.getName() << endl;
            cout << "   Location: (" << current->node.getLatitude() << ", " << current->node.getLongitude() << ")" << endl;

            Edge* edge = current->edges;
            int edgeCount = 0;
            while (edge) {
                edgeCount++;
                edge = edge->next;
            }
            cout << "   Connections: " << edgeCount << endl;
            cout << endl;
        }
        current = current->next;
    }

    if (count == 0) {
        cout << "No nodes of type '" << nodeType << "' found in the network." << endl;
    }
    else {
        cout << "Total " << nodeType << " nodes: " << count << endl;
    }
}

void Graph::exportNodesCSV(const string& filename) {
    ofstream file(filename.c_str());

    if (!file.is_open()) {
        cout << "Error: Could not create " << filename << endl;
        return;
    }

    // Header
    file << "id,name,sector,lat,lon\n";

    GraphNode* current = head;

    while (current) {
        Node& n = current->node;

        file << n.getNodeID() << "," << n.getName() << "," << n.getNodeType() << "," << n.getLatitude() << "," << n.getLongitude() << "\n";

        current = current->next;
    }

    file.close();

    cout << "Nodes exported to " << filename << endl;
}

void Graph::exportEdgesCSV(const string& filename) {
    ofstream file(filename.c_str());

    if (!file.is_open()) {
        cout << "Error: Could not create " << filename << endl;
        return;
    }

    // Header
    file << "from,to,weight_km\n";

    GraphNode* current = head;

    while (current) {
        string srcID = current->node.getNodeID();

        Edge* edge = current->edges;

        while (edge) {
            // one row per edge
            file << srcID << "," << edge->destNodeID << "," << edge->weight << "\n";

            edge = edge->next;
        }

        current = current->next;
    }

    file.close();

    cout << "Edges exported to " << filename << endl;
}
