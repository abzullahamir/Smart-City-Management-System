#ifndef COMMERCIALSYSTEM_H
#define COMMERCIALSYSTEM_H

#include "MallHashTable.h"
#include "Mall.h"
#include "Product.h"
#include "Graph.h"

#include <string>
using namespace std;

class CommercialSystem {
private:
    Graph* cityGraph;       // Shared city graph (reference)
    MallHashTable* mallHash;
    bool ownsGraph;         // Does this system solely own the graph

    string cleanString(string str);
    string toLowerCase(string str);
    void parseMallsFile(string filename);
    void parseProductsFile(string filename);
    void parseCoordinates(string coordStr, double& lat, double& lon);
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    bool validateCoordinates(double lat, double lon);
    bool validateString(string str);

    // Category management (dynamic)
    CategoryList* getAllCategories();  // Get categories from all malls
    string selectCategoryFromMall(string mallID);  // Select from specific mall
    string selectCategoryFromAll();  // Select from all malls

public:
    CommercialSystem(Graph* sharedGraph = nullptr);
    ~CommercialSystem();

    void loadDataFromFiles(string mallsFile, string productsFile);

    Graph* getCityGraph();

    // Mall operations
    void registerMall(string mallID, string name, string location, double lat, double lon);
    Mall* getMallByID(string mallID);
    void displayAllMalls();
    void displayMallDetails(string mallID);
    void displayNetwork();

    // Product operations
    void addProductToMall(string mallID, string productID, string name, string category, double price, int quantity);
    void searchProductInMall(string mallID, string productID);
    void searchProductsByCategory(string mallID, string category);
    void displayMallProducts(string mallID);

    // Searching
    void findNearestMall(double userLat, double userLon);
    void findRouteToMall(string fromNodeID, string mallID);

    // Category search across all the malls
    void searchCategoryInAllMalls(string category);

    // Display categories
    void displayAllCategories();

    // Menu
    void displayMenu();
    void run();
};

#endif