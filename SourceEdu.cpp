#include <iostream>
#include <string>
#include "educationSystem.h"   // header where your educationSystem class is defined

using namespace std;

int main() {
    educationSystem edu;

    // --------- LOAD DATA FROM CSV FILES ----------
    // Make sure these files are in the working directory or give full paths
    string schoolsFile = "schools.csv";                 // SchoolID,Name,Sector,Rating,Lat,Lon,Subjects...
    string departmentsFile = "department.csv";  // SchoolID,DepartmentName,DepartmentID,ProfessorName,ProfessorID
    string classesFile = "classRoom.csv";        // SchoolID,DepartmentName,ClassName,ClassID,StudentName

    edu.loadEducationData(schoolsFile, departmentsFile, classesFile);

    // --------- BASIC AUTOMATIC TESTS ----------
    cout << "\n=== Automatic sanity checks ===\n";

    // 1) Search a known school
    cout << "\n[1] Search for school S01:\n";
    edu.searchSchool("S01");

    // 2) Check a subject query (if your CSV has these subjects)
    cout << "\n[2] Schools offering 'Math':\n";
    edu.searchSubject("Math");

    // 3) Show min/max rated schools from heap
    cout << "\n[3] Lowest rated school:\n";
    edu.showLowRatedSchool();

    cout << "\n[4] Top rated school:\n";
    edu.showTopSchool();

    // 4) Display full hierarchy (tree structure) for all schools
    cout << "\n[5] Full education hierarchy:\n";
    edu.display();

    // 5) Manual interactive menu (optional but nice for viva)
    while (true) {
        cout << "\n===== Education System Menu =====\n";
        cout << "1. Register new school\n";
        cout << "2. Register new department\n";
        cout << "3. Register new class\n";
        cout << "4. Register new student\n";
        cout << "5. Search school by ID\n";
        cout << "6. Show all schools hierarchy\n";
        cout << "7. Show lowest rated school\n";
        cout << "8. Show top rated school\n";
        cout << "9. Search schools by subject\n";
        cout << "0. Exit\n";
        cout << "Select option: ";

        int choice;
        if (!(cin >> choice)) break;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) break;

        string id, name, sector, dept, cls, subj;
        float rating, lat, lon;
        string subjectsArr[5];
        int subCount;

        switch (choice) {
        case 1:
            cout << "Enter School ID: ";
            getline(cin, id);
            cout << "Enter School Name: ";
            getline(cin, name);
            cout << "Enter Sector: ";
            getline(cin, sector);
            cout << "Enter Rating (0-5): ";
            cin >> rating;
            cout << "Enter Latitude: ";
            cin >> lat;
            cout << "Enter Longitude: ";
            cin >> lon;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "How many subjects (max 5)? ";
            cin >> subCount;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (subCount > 5) subCount = 5;
            for (int i = 0; i < subCount; ++i) {
                cout << "Subject " << i + 1 << ": ";
                getline(cin, subjectsArr[i]);
            }

            edu.registerSchool(name, id, sector, rating, lat, lon, subjectsArr, subCount);
            break;

        case 2:
            cout << "Enter School ID: ";
            getline(cin, id);
            cout << "Enter Department Name: ";
            getline(cin, dept);
            edu.registerDepartment(id, dept);
            break;

        case 3:
            cout << "Enter School ID: ";
            getline(cin, id);
            cout << "Enter Department Name: ";
            getline(cin, dept);
            cout << "Enter Class Name: ";
            getline(cin, cls);
            edu.registerClass(id, dept, cls);
            break;

        case 4:
            cout << "Enter School ID: ";
            getline(cin, id);
            cout << "Enter Department Name: ";
            getline(cin, dept);
            cout << "Enter Class Name: ";
            getline(cin, cls);
            cout << "Enter Student Name: ";
            getline(cin, name);
            edu.addStudent(id, dept, cls, name);
            break;

        case 5:
            cout << "Enter School ID to search: ";
            getline(cin, id);
            edu.searchSchool(id);
            break;

        case 6:
            edu.display();
            break;

        case 7:
            edu.showLowRatedSchool();
            break;

        case 8:
            edu.showTopSchool();
            break;

        case 9:
            cout << "Enter subject name: ";
            getline(cin, subj);
            edu.searchSubject(subj);
            break;

        default:
            cout << "Invalid option.\n";
            break;
        }
    }

    cout << "Exiting program.\n";
    return 0;
}
