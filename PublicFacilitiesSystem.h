#ifndef PUBLIC_FACILITIES_SYSTEM_H
#define PUBLIC_FACILITIES_SYSTEM_H

#include "Graph.h"
#include "publicFacility.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

const double PI_FAC = 3.14159265358979323846;
const double EARTH_RADIUS_FAC = 6371.0;

class PublicFacilitiesSystem {
private:
    Graph* cityGraph;
    PublicFacility* facilityHead;
    int facilityCount;
    bool ownsGraph;

    double calculateDistance(double lat1, double lon1,
        double lat2, double lon2);
    string cleanString(string str);
    void parseCoordinates(string coordStr, double& lat, double& lon);
    PublicFacility* findFacilityByID(const string& id);

    string toLower(const string& s) const;
    string normalizeType(const string& type) const;

public:
    PublicFacilitiesSystem(Graph* sharedGraph = nullptr);
    ~PublicFacilitiesSystem();

    Graph* getCityGraph();

    void loadPublicFacilities(string filename);
    void registerFacility(string id, string name, string type,
        string sector, double lat, double lon);

    void displayAllFacilities();
    void displayFacilitiesByType(string type);
    void viewFacility(string facilityID);

    PublicFacility* findNearestFacility(double lat, double lon,
        string typeFilter = "ANY");
    void findNearestFacilityByName(string name);
    void getTopNearestFacilities(double lat, double lon,
        int topN, string typeFilter = "ANY");
    void findRouteToFacility(string fromNodeID, string facilityID);

    void displayMenu();
    void run();
};

#endif // PUBLIC_FACILITIES_SYSTEM_H
