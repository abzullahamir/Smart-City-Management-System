#ifndef MEDICALSYSTEM_H
#define MEDICALSYSTEM_H

#include "Graph.h"
#include "hospitals.h"
#include "hospitalHashTable.h"
#include "doctorsHashtable.h"
#include "eBedsMaxHeap.h"
#include "medicine.h"
#include "pharmacy.h"
#include "pharmacyHashtable.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

// constants for distance calculation
const double PI_MED = 3.14159265358979323846;
const double EARTH_RADIUS_MED = 6371.0; // km

// pharmacy linked list node for module
struct PharmacyNode {
    pharmacy* pharm;
    PharmacyNode* next;

    PharmacyNode(pharmacy* p) : pharm(p), next(nullptr) {}
};

class MedicalSystem {
private:
    Graph* cityGraph;               // shared city graph
    HospitalHashTable* hospitalHash; // hospital lookup by id
    DoctorHashTable* doctorHash;    // doctor lookup by specialization
    HospitalMaxHeap* bedsHeap;      // emergency beds priority queue
    PharmacyNode* pharmacyHead;     // pharmacy linked list
    int pharmacyCount;
    bool ownsGraph;                 // does this system own the graph?

    // helper functions
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    string cleanString(string str);
    void parseCoordinates(string coordStr, double& lat, double& lon);
    int stringToInt(string str);
    PharmacyNode* findPharmacyNode(const string& pharmacyID);

    // string helpers for case insensitivity
    string toLower(const string& s);
    bool equalsIgnoreCase(const string& a, const string& b);
    bool containsIgnoreCase(const string& text, const string& pattern);

public:
    MedicalSystem(Graph* sharedGraph = nullptr,
        int hospitalHashSize = 20,
        int doctorHashSize = 30,
        int heapCapacity = 50);

    ~MedicalSystem();

    Graph* getCityGraph();

    // ============ DATA LOADING ============
    void loadDataFromFiles(string hospitalsFile, string pharmaciesFile);
    void loadHospitals(string filename);
    void loadPharmacies(string filename);

    // ============ HOSPITAL MANAGEMENT ============
    void registerHospital(string id, string name, string sector,
        int beds, string spec, double lat, double lon);
    void deactivateHospital(string hospitalID);
    void reactivateHospital(string hospitalID);
    void viewHospital(string hospitalID);
    void displayAllHospitals();
    void displayNetwork();

    // ============ DOCTOR MANAGEMENT ============
    void hireDoctor(string hospitalID, string doctorName, string specialization);
    void fireDoctor(string hospitalID, string doctorName);
    void viewAllDoctors();
    void viewDoctorsBySpecialization(string specialization);
    void viewDoctorsAtHospital(string hospitalID);
    int countDoctorsBySpecialization(string specialization);
    void findHospitalsWithSpecialization(string specialization);

    // ============ PATIENT MANAGEMENT ============
    void admitPatient(string hospitalID, string patientName, int age, string condition);
    void dischargePatient(string hospitalID, string patientName);
    void viewPatientsAtHospital(string hospitalID);
    void viewAllPatients();
    void transferPatient(string fromHospitalID, string toHospitalID, string patientName);
    void findPatient(string patientName);
    void getPatientStatistics(string hospitalID);

    // ============ EMERGENCY BED MANAGEMENT ============
    void viewMaxEmergencyBeds();
    void viewAllEmergencyBeds();
    void changeEmergencyBedCount(string hospitalID, int newBedCount);
    void reserveEmergencyBed(string hospitalID);   // asks how many beds to reserve
    void releaseEmergencyBed(string hospitalID);   // asks how many beds to release
    void getEmergencyBedStatistics();
    void findHospitalsWithMinimumBeds(int minBeds);
    void alertLowBedAvailability(int threshold = 3);

    // ============ NEAREST HOSPITAL & ROUTING ============
    Hospital* findNearestHospital(double lat, double lon);
    Hospital* findNearestHospitalWithBeds(double lat, double lon, int minBeds);
    Hospital* findNearestHospitalWithSpecialization(double lat, double lon, string specialization);
    Hospital* findNearestHospitalWithDoctorSpecialization(double lat, double lon, string doctorSpec);
    Hospital* findOptimalHospital(double lat, double lon, string specialization, int minBeds);
    void getTopNearestHospitals(double lat, double lon, int topN);
    void findRouteToHospital(string fromNodeID, string hospitalID);

    // ============ PHARMACY MANAGEMENT ============
    void registerPharmacy(string id, string name, string sector,
        double lat, double lon, Medicine initialMed);
    void registerPharmacyEmpty(string id, string name, string sector,
        double lat, double lon);
    void removePharmacy(string pharmacyID);
    void displayAllPharmacies();
    void viewPharmacy(string pharmacyName);
    void viewPharmacyByID(string pharmacyID);
    void addMedicineToPharmacy(string pharmacyID, string medName,
        string formula, int price);
    void removeMedicineFromPharmacy(string pharmacyID, string medicineName);
    int getPharmacyCount();

    // ============ MEDICINE SEARCH & PRICING ============
    void searchMedicine(string medicineName);
    void searchMedicineByFormula(string formula);
    void findCheapestPharmacyForMedicine(string medicineName);
    pharmacy* findNearestPharmacy(double lat, double lon);
    pharmacy* findNearestPharmacyWithMedicine(double lat, double lon, string medicineName);
    void compareMedicinePrices(string medicineName);
    void getTopNearestPharmacies(double lat, double lon, int topN);
    void findRouteToPharmacy(string fromNodeID, string pharmacyID);

    // ============ MENU SYSTEM ============
    void displayMenu();
    void run();
};

#endif // MEDICALSYSTEM_H
