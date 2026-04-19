#include "CommercialSystem.h"

#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

const double PI = 3.14159265358979323846;
const double EARTH_RADIUS = 6371.0; // km

CommercialSystem::CommercialSystem(Graph* sharedGraph) {
    if (sharedGraph) {
        cityGraph = sharedGraph;
        ownsGraph = false;
    }
    else {
        cityGraph = new Graph();
        ownsGraph = true;
    }
    mallHash = new MallHashTable(101);
}

CommercialSystem::~CommercialSystem() {
    if (ownsGraph) {
        delete cityGraph;
    }
    delete mallHash;
}

Graph* CommercialSystem::getCityGraph() {
    return cityGraph;
}

string CommercialSystem::cleanString(string str) {
    size_t first = str.find_first_not_of(" \t\r\n"); // Remove leading whitespace

    if (first == string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\r\n"); // Remove trailing whitespace
    
    return str.substr(first, last - first + 1);
}

string CommercialSystem::toLowerCase(string str) {
    string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] + 32;
        }
    }
    return result;
}

bool CommercialSystem::validateCoordinates(double lat, double lon) {
    if (lat < -90.0 || lat > 90.0) {
        cout << "Invalid latitude! Must be between -90 and +90." << endl;
        return false;
    }
    if (lon < -180.0 || lon > 180.0) {
        cout << "Invalid longitude! Must be between -180 and +180." << endl;
        return false;
    }
    return true;
}

bool CommercialSystem::validateString(string str) {
    if (str.empty()) return false;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
            return true;
        }
    }
    return false;
}

CategoryList* CommercialSystem::getAllCategories() {
    CategoryList* allCategories = new CategoryList(100);

    int mallCount = 0;
    Mall** allMalls = mallHash->getAllMalls(mallCount);

    if (allMalls) {
        for (int i = 0; i < mallCount; i++) {
            int productCount = 0;
            Product** products = allMalls[i]->getProducts()->getAllProducts(productCount);

            if (products) {
                for (int j = 0; j < productCount; j++) {
                    allCategories->add(products[j]->getCategory());
                }
                delete[] products;
            }
        }
        delete[] allMalls;
    }

    return allCategories;
}

string CommercialSystem::selectCategoryFromMall(string mallID) {
    Mall* mall = mallHash->search(mallID);
    if (!mall) {
        cout << "Mall not found!" << endl;
        return "";
    }

    CategoryList* categories = mall->getAvailableCategories();

    if (categories->count == 0) {
        cout << "No products in this mall yet!" << endl;
        delete categories;
        return "";
    }

    cout << endl << "===== Categories Available in " << mall->getName() << " =====" << endl;
    categories->display();
    cout << "========================================" << endl;
    cout << endl << "Select category (1-" << categories->count << ") or enter custom: ";

    string input;
    getline(cin, input);
    input = cleanString(input);

    // Check if it's a number
    bool isNumber = true;
    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] < '0' || input[i] > '9') {
            isNumber = false;
            break;
        }
    }

    if (isNumber && !input.empty()) {
        int choice = 0;
        for (size_t i = 0; i < input.length(); i++) {
            choice = choice * 10 + (input[i] - '0');
        }

        if (choice >= 1 && choice <= categories->count) {
            string selected = categories->categories[choice - 1];
            delete categories;
            return selected;
        }
    }

    // Custom category or invalid selection
    delete categories;

    if (validateString(input)) {
        return input;
    }

    return "";
}

string CommercialSystem::selectCategoryFromAll() {
    CategoryList* categories = getAllCategories();

    if (categories->count == 0) {
        cout << "No products in any mall yet!" << endl;
        delete categories;
        return "";
    }

    cout << endl << "===== All Available Categories =====" << endl;
    categories->display();
    cout << "========================================" << endl;
    cout << endl << "Select category (1-" << categories->count << ") or enter custom: ";

    string input;
    getline(cin, input);
    input = cleanString(input);

    // Check if it's a number
    bool isNumber = true;
    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] < '0' || input[i] > '9') {
            isNumber = false;
            break;
        }
    }

    if (isNumber && !input.empty()) {
        int choice = 0;
        for (size_t i = 0; i < input.length(); i++) {
            choice = choice * 10 + (input[i] - '0');
        }

        if (choice >= 1 && choice <= categories->count) {
            string selected = categories->categories[choice - 1];
            delete categories;
            return selected;
        }
    }

    // Custom category
    delete categories;

    if (validateString(input)) {
        return input;
    }

    return "";
}

void CommercialSystem::displayAllCategories() {
    cout << endl << "===== All Product Categories in System =====" << endl;

    CategoryList* categories = getAllCategories();

    if (categories->count == 0) {
        cout << "No products in system yet." << endl;
    }
    else {
        categories->display();
        cout << endl << "Total unique categories: " << categories->count << endl;
    }

    delete categories;
}

void CommercialSystem::parseCoordinates(string coordStr, double& lat, double& lon) {
    lat = 0.0;
    lon = 0.0;

    if (!coordStr.empty() && coordStr[0] == '"') { // Remove quotation marks
        coordStr = coordStr.substr(1, coordStr.length() - 2);
    }

    size_t commaPos = coordStr.find(',');
    if (commaPos != string::npos) {
        string latStr = cleanString(coordStr.substr(0, commaPos));
        string lonStr = cleanString(coordStr.substr(commaPos + 1));

        try {
            lat = stod(latStr);
            lon = stod(lonStr);
        }
        catch (...) {
            cout << "Warning: Could not parse coordinates: " << coordStr << endl;
            lat = 0.0;
            lon = 0.0;
        }
    }
}

double CommercialSystem::calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Convert degree difference into radians
    double diffLat = (lat2 - lat1) * PI / 180.0;
    double diffLon = (lon2 - lon1) * PI / 180.0;

    // Convert latitude to radians
    lat1 = lat1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;

    // Haversine formula
    double a = sin(diffLat / 2) * sin(diffLat / 2) + sin(diffLon / 2) * sin(diffLon / 2) * cos(lat1) * cos(lat2);

    // Convert to angle (C = 2 * arcsin( sqrt(a) )), but safer version
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    // Convert angle to distance (S = r * theta)
    return EARTH_RADIUS * c;
}

void CommercialSystem::parseMallsFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header

    int lineNum = 1;
    while (getline(file, line)) {
        lineNum++;
        if (line.empty()) continue;

        string mallID = "";
        string name = "";
        string location = "";
        string coordStr = "";

        int fieldCount = 0;
        bool inQuotes = false;
        string currentField = "";

        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];

            if (c == '"') {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes) {
                if (fieldCount == 0) {
                    mallID = cleanString(currentField);
                }
                else if (fieldCount == 1) {
                    name = cleanString(currentField);
                }
                else if (fieldCount == 2) {
                    location = cleanString(currentField);
                }
                currentField = "";
                fieldCount++;
            }
            else {
                currentField += c;
            }
        }

        coordStr = cleanString(currentField);

        double lat, lon;
        parseCoordinates(coordStr, lat, lon);

        registerMall(mallID, name, location, lat, lon);
    }

    file.close();
    cout << "Loaded " << (lineNum - 1) << " malls from " << filename << endl;
}

void CommercialSystem::parseProductsFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header

    int lineNum = 1;
    while (getline(file, line)) {
        lineNum++;
        if (line.empty()) continue;

        string mallID = "";
        string productID = "";
        string name = "";
        string category = "";
        string priceStr = "";
        string qtyStr = "";

        int fieldCount = 0;
        string currentField = "";

        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];

            if (c == ',') {
                if (fieldCount == 0) {
                    mallID = cleanString(currentField);
                }
                else if (fieldCount == 1) {
                    productID = cleanString(currentField);
                }
                else if (fieldCount == 2) {
                    name = cleanString(currentField);
                }
                else if (fieldCount == 3) {
                    category = cleanString(currentField);
                }
                else if (fieldCount == 4) {
                    priceStr = cleanString(currentField);
                }
                currentField = "";
                fieldCount++;
            }
            else {
                currentField += c;
            }
        }

        qtyStr = cleanString(currentField);

        try {
            double price = stod(priceStr);
            int quantity = stoi(qtyStr);
            addProductToMall(mallID, productID, name, category, price, quantity);
        }
        catch (...) {
            cout << "Warning: Could not parse product on line " << lineNum << endl;
        }
    }

    file.close();
    cout << "Loaded products from " << filename << endl;
}

void CommercialSystem::loadDataFromFiles(string mallsFile, string productsFile) {
    cout << endl << "===== Loading Commercial Data =====" << endl;
    
    parseMallsFile(mallsFile);
    parseProductsFile(productsFile);

    cout << "Commercial system initialized successfully!" << endl;
}

void CommercialSystem::registerMall(string mallID, string name, string location, double lat, double lon) {
    Mall* mall = new Mall(mallID, name, location, lat, lon);
    mallHash->insert(mallID, mall);

    // Add mall as node to city graph
    Node mallNode(mallID, name, "MALL", lat, lon);
    cityGraph->addNode(mallNode);

    // Connect mall to nearby bus stops (within 1.5 km distance)
    cityGraph->connectToNearbyNodes(mallID, 1.5, "BUS_STOP");

    cout << "Mall " << mallID << " registered and added to city graph successfully!" << endl;
}

Mall* CommercialSystem::getMallByID(string mallID) {
    return mallHash->search(mallID);
}

void CommercialSystem::displayAllMalls() {
    cout << endl << "===== All Malls =====" << endl;
    mallHash->display();
}

void CommercialSystem::displayNetwork() {
    cout << endl << "========== Commercial Sector Graph ==========" << endl;
    cityGraph->displayGraph("MALL");
}

void CommercialSystem::displayMallDetails(string mallID) {
    Mall* mall = mallHash->search(mallID);
    if (mall) {
        mall->display();
    }
    else {
        cout << "Mall with ID '" << mallID << "' not found!" << endl;
    }
}

void CommercialSystem::addProductToMall(string mallID, string productID, string name, string category, double price, int quantity) {
    Mall* mall = mallHash->search(mallID);
    if (mall) {
        Product* product = new Product(productID, name, category, price, quantity);
        mall->addProduct(product);
    }
    else {
        cout << "Mall with ID '" << mallID << "' not found!" << endl;
    }
}

void CommercialSystem::searchProductInMall(string mallID, string productID) {
    Mall* mall = mallHash->search(mallID);
    if (mall) {
        Product* product = mall->searchProduct(productID);
        if (product) {
            cout << endl << "Product found in " << mall->getName() << ":" << endl;
            product->display();
        }
        else {
            cout << "Product '" << productID << "' not found in " << mall->getName() << endl;
        }
    }
    else {
        cout << "Mall with ID '" << mallID << "' not found!" << endl;
    }
}

void CommercialSystem::searchProductsByCategory(string mallID, string category) {
    Mall* mall = mallHash->search(mallID);
    if (!mall) {
        cout << "Mall with ID '" << mallID << "' not found!" << endl;
        return;
    }

    cout << endl << "===== Searching in " << mall->getName() << " =====" << endl;

    string lowerCategory = toLowerCase(category);
    ProductHashTable* products = mall->getProducts();
    int productCount = 0;
    Product** allProducts = products->getAllProducts(productCount);

    bool found = false;

    if (allProducts) {
        cout << "Category: " << category << endl;
        cout << "----------------------------------------" << endl;

        for (int i = 0; i < productCount; i++) {
            string productCategory = toLowerCase(allProducts[i]->getCategory());

            if (productCategory == lowerCategory) {
                if (!found) found = true;
                allProducts[i]->display();
                cout << endl;
            }
        }
        delete[] allProducts;
    }

    if (!found) {
        cout << "No products found in category '" << category << "'." << endl;
    }
}

void CommercialSystem::displayMallProducts(string mallID) {
    Mall* mall = mallHash->search(mallID);
    if (mall) {
        mall->displayProducts();
    }
    else {
        cout << "Mall with ID '" << mallID << "' not found!" << endl;
    }
}

void CommercialSystem::findNearestMall(double userLat, double userLon) {
    cout << endl << "===== Finding Nearest Mall =====" << endl;

    // Use graph to find nearest mall node
    string nearestMallID = cityGraph->findNearestNode(userLat, userLon, "MALL");

    if (nearestMallID.empty()) {
        cout << "No malls registered in the system!" << endl;
        return;
    }

    Mall* nearestMall = mallHash->search(nearestMallID);
    if (!nearestMall) {
        cout << "Mall data not found!" << endl;
        return;
    }

    double distance = calculateDistance(userLat, userLon, nearestMall->getLatitude(), nearestMall->getLongitude());

    cout << "Nearest Mall: " << nearestMall->getName() << endl;
    cout << "Location: " << nearestMall->getLocation() << endl;
    cout << "Distance: " << distance << " km" << endl;
    cout << "Total Products: " << nearestMall->getProductCount() << endl;
}

void CommercialSystem::findRouteToMall(string fromNodeID, string mallID) {
    cout << endl << "===== Finding Route to Mall =====" << endl;

    Mall* mall = mallHash->search(mallID);
    if (!mall) {
        cout << "Mall not found!" << endl;
        return;
    }

    cout << "Finding route to: " << mall->getName() << " (" << mall->getLocation() << ")" << endl;

    // Use Dijkstra with no specific node filter (can use any roads or nodes)
    cityGraph->dijkstraAlgo(fromNodeID, mallID, "ANY");
}

void CommercialSystem::searchCategoryInAllMalls(string category) {
    if (!validateString(category)) {
        cout << "Category cannot be empty!" << endl;
        return;
    }

    cout << endl << "===== Searching Category Across All Malls =====" << endl;
    cout << "Category: " << category << endl;

    string lowerCategory = toLowerCase(category);
    int mallCount = 0;
    Mall** allMalls = mallHash->getAllMalls(mallCount);

    if (allMalls == nullptr || mallCount == 0) {
        cout << "No malls registered!" << endl;
        return;
    }

    bool found = false;
    for (int i = 0; i < mallCount; i++) {
        ProductHashTable* products = allMalls[i]->getProducts();

        int productCount = 0;
        Product** allProducts = products->getAllProducts(productCount);

        if (allProducts) {
            bool mallHasCategory = false;
            for (int j = 0; j < productCount; j++) {
                string productCategory = toLowerCase(allProducts[j]->getCategory());

                if (productCategory == lowerCategory) {
                    if (!mallHasCategory) {
                        cout << endl << allMalls[i]->getName() << " (" << allMalls[i]->getLocation() << ")" << endl;
                        cout << "----------------------------------------" << endl;
                        mallHasCategory = true;
                        found = true;
                    }
                    allProducts[j]->display();
                    cout << endl;
                }
            }
            delete[] allProducts;
        }
    }

    if (!found) {
        cout << "No products found in category '" << category << "' across all malls." << endl;
    }

    delete[] allMalls;
}

void CommercialSystem::displayMenu() {
    cout << endl << "========================================" << endl;
    cout << "     SMART CITY COMMERCIAL SYSTEM" << endl;
    cout << "========================================" << endl;
    cout << "1.  Register New Mall" << endl;
    cout << "2.  Add Product to Mall" << endl;
    cout << "3.  Search Product in Specific Mall" << endl;
    cout << "4.  Search by Category in Mall" << endl;
    cout << "5.  Display All Malls (HashTable)" << endl;
    cout << "6.  Display Mall Details" << endl;
    cout << "7.  Display Mall Products" << endl;
    cout << "8.  Find Nearest Mall" << endl;
    cout << "9.  Find Route to Mall" << endl;
    cout << "10.  Search Category Across All Malls" << endl;
    cout << "11.  Display Network" << endl;
    cout << "12.  View Product Categories" << endl;
    cout << "0.  Exit" << endl;
    cout << "========================================" << endl;
    cout << "Enter choice: ";
}

void CommercialSystem::run() {
    int choice;

    while (true) {
        displayMenu();
        
        if (!(cin >> choice)) {
            cout << endl << "Invalid input! Please enter a number." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();

        if (choice < 0 || choice > 12) {
            cout << endl << "Invalid choice! Please select 0-12." << endl;
            continue;
        }

        if (choice == 0) {
            cout << "Exiting Commercial System..." << endl;
            break;
        }

        switch (choice) {
        case 1: {
            string mallID, name, location;
            double lat, lon;
            cout << "Enter mall ID: ";
            getline(cin, mallID);
            cout << "Enter mall name: ";
            getline(cin, name);
            cout << "Enter location: ";
            getline(cin, location);
            cout << "Enter latitude: ";
            cin >> lat;
            cout << "Enter longitude: ";
            cin >> lon;
            cin.ignore();
            registerMall(mallID, name, location, lat, lon);
            cout << "Mall registered successfully!" << endl;
            break;
        }
        case 2: {
            string mallID, productID, name, category;
            double price;
            int quantity;
            cout << "Enter mall ID: ";
            getline(cin, mallID);
            cout << "Enter product ID: ";
            getline(cin, productID);
            cout << "Enter product name: ";
            getline(cin, name);
            cout << "Enter category: ";
            getline(cin, category);
            cout << "Enter price: ";
            cin >> price;
            cout << "Enter quantity: ";
            cin >> quantity;
            cin.ignore();
            addProductToMall(mallID, productID, name, category, price, quantity);
            cout << "Product added successfully!" << endl;
            break;
        }
        case 3: {
            string mallID, productID;
            cout << "Enter mall ID: ";
            getline(cin, mallID);
            cout << "Enter product ID: ";
            getline(cin, productID);
            searchProductInMall(mallID, productID);
            break;
        }
        case 4: {
            string mallID, category;
            cout << "Enter mall ID: ";
            getline(cin, mallID);
            cout << "Enter category: ";
            getline(cin, category);
            searchProductsByCategory(mallID, category);
            break;
        }
        case 5: {
            displayAllMalls();
            break;
        }
        case 6: {
            string mallID;
            cout << "Enter mall ID: ";
            getline(cin, mallID);
            displayMallDetails(mallID);
            break;
        }
        case 7: {
            string mallID;
            cout << "Enter mall ID: ";
            getline(cin, mallID);
            displayMallProducts(mallID);
            break;
        }
        case 8: {
            double lat, lon;
            cout << "Enter your latitude: ";
            cin >> lat;
            cout << "Enter your longitude: ";
            cin >> lon;
            cin.ignore();
            findNearestMall(lat, lon);
            break;
        }
        case 9: {
            string fromNode, mallID;
            cout << "Enter your current location (Node ID): ";
            getline(cin, fromNode);
            cout << "Enter mall ID: ";
            getline(cin, mallID);
            findRouteToMall(fromNode, mallID);
            break;
        }
        case 10: {
            string category;
            cout << "Enter category to search: ";
            getline(cin, category);
            searchCategoryInAllMalls(category);
            break;
        }
        case 11: {
            displayNetwork();
            break;
        }
        case 12: {
            displayAllCategories();
            break;
        }
        default:
            cout << "Invalid choice! Try again." << endl;
        }
    }
}