#pragma once
#include "familyTree.h"
#include "populationTree.h"

#include <iostream>
#include <string>
using namespace std;

struct PopulationStats {
    
    int males = 0;
    int females = 0;

 
    int child = 0;   // 0-18
    int young = 0;   // 19-35
    int middle = 0;  // 36-60
    int senior = 0;  //60+

    // 3. Population Density (People count)
    int totalPopulation = 0;
    int houseCount = 0;

   
    int students = 0;
    int professionals = 0;
    int others = 0;

    PopulationStats() {}


    void collectFamilyDataFromSector(sectorNode* sector) {
        if (!sector) return;

        streetNode* streetTemp = sector->streetHead;
        while (streetTemp) {
            houseNode* houseTemp = streetTemp->houseHead;
            while (houseTemp) {
                houseCount++;
                personNode* familyHead = houseTemp->familyHead;
                collectFamilyData(familyHead);
                houseTemp = houseTemp->next;
            }
            streetTemp = streetTemp->next;
        }
    }
    void collectFamilyData(personNode* p) {
        if (!p) return;

        totalPopulation++;

        // Gender
        if (p->gender == 'M' || p->gender == 'm') 
            males++;
        else 
            females++;

        // Age
        if (p->age <= 18)
            child++;
        else if (p->age <= 35) 
            young++;
        else if (p->age <= 60) 
            middle++;
        else 
            senior++;

        // Occupation
        if (p->occupation == "Student" || p->occupation == "student")
            students++;
        else if (p->occupation == "None" || p->occupation == "Retired") 
            others++;
        else professionals++; // Assume everyone else is working

        if (p->spouse)
            collectFamilyData(p->spouse);
        if (p->childHead)
            collectFamilyData(p->childHead);
        if (p->next)
            collectFamilyData(p->next);

    }



    void printReport(string title) {
        cout << "\n==============================================" << endl;
        cout << "  " << title << endl;
        cout << "==============================================" << endl;

        cout << " [1] POPULATION DENSITY" << endl;
        cout << "     Total People: " << totalPopulation << endl;
        cout << "     Total Houses: " << houseCount << endl;
        if (houseCount > 0)
            cout << "     Avg People/House: " << (float)totalPopulation / houseCount << endl;
        else
            cout << "     Avg People/House: 0" << endl;

        cout << "\n [2] GENDER RATIO" << endl;
        cout << "     Male:   " << males << " (" << (totalPopulation ? (males * 100 / totalPopulation) : 0) << "%)" << endl;
        cout << "     Female: " << females << " (" << (totalPopulation ? (females * 100 / totalPopulation) : 0) << "%)" << endl;

        cout << "\n [3] AGE DISTRIBUTION" << endl;
        cout << "     Children (0-18):   " << child << endl;
        cout << "     Youth (19-35):     " << young << endl;
        cout << "     Middle Age (36-60):" << middle << endl;
        cout << "     Seniors (60+):     " << senior << endl;

        cout << "\n [4] OCCUPATION SUMMARY" << endl;
        cout << "     Students:      " << students << endl;
        cout << "     Professionals: " << professionals << endl;
        cout << "     Unemployed/Other: " << others << endl;
        cout << "==============================================\n" << endl;
    }
};