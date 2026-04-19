#pragma once
#include "Graph.h"
#include "minHeap_Education.h"
#include "stringHash.h"
#include "subjectHashTable.h"
#include "Education_Tree.h"

#include <iostream>
#include <string>
#include <fstream>   
using namespace std;


const double PI = 3.14159265358979323846;
const double EARTH_RADIUS = 6371.0; // km
class educationSystem {
	schoolNode* root; // root/first node of the tree
	subjectHashTable * subjectTable;
	minHeap_Education* schoolHeap;
	HastTable* schoolTable;

	Graph* educationGraph; // shared graph for the city
	bool ownsGraph;

	string getNextField(const string& line, size_t& startPos) {
		if (startPos >= line.size()) return "";
		size_t commaPos = line.find(',', startPos);
		string field;
		if (commaPos == string::npos) {
			field = line.substr(startPos);
			startPos = line.size();
		}
		else {
			field = line.substr(startPos, commaPos - startPos);
			startPos = commaPos + 1;
		}
		
		while (!field.empty() && (field[0] == ' ' || field[0] == '\t'))
			field.erase(field.begin());
		while (!field.empty() && (field.back() == ' ' || field.back() == '\t' || field.back() == '\r'))
			field.pop_back();
		return field;
	}

	string readNonEmpty( string prompt) {
		string s;
		while (true) {
			cout << prompt;
			getline(cin, s);
			if (!s.empty()) return s;
			cout << "[!] Input cannot be empty. Try again.\n";
		}
	}

public:
	educationSystem(Graph* graph = NULL) {
		root = nullptr;
		schoolHeap = new minHeap_Education();
		schoolTable = new HastTable();
		subjectTable = new subjectHashTable();

		if (graph) {
			educationGraph = graph;
			ownsGraph = false;
		}
		else {
			educationGraph = new Graph();
			ownsGraph = true;
		}
	}

	
	~educationSystem() {
		if (schoolHeap) delete(schoolHeap);
		if (schoolTable) delete(schoolTable);
		if (subjectTable) delete(subjectTable);

		if (ownsGraph) {
			delete (educationGraph);
		}

	}

	void addClass(string schoolID, string deptName, string className) {
		schoolNode* school = schoolTable->search(schoolID);
		if (school) {
			departmentNode* dept = school->getDepartment(deptName);
			if (dept)
				dept->addClass(className);
			else
				cout << "Department " << deptName << " not found in school " << schoolID << "." << endl;
		}
		else {
			cout << "School with ID " << schoolID << " not found." << endl;
		}
	}
	
	

	void addStudent(string schoolID, string deptName, string className, string studentName) {
		schoolNode* school = schoolTable->search(schoolID);
		if (school) {
			departmentNode* dept = school->getDepartment(deptName);
			if (dept) {
				classNode* classN = dept->getClass(className);
				if (classN) {
					classN->addStudent(studentName);
				}
				else {
					cout << "Class " << className << " not found in Department " << deptName << "." << endl;
				}
			}
			else {
				cout << "Department " << deptName << " not found in school " << schoolID << "." << endl;
			}
		}
		else {
			cout << "School with ID " << schoolID << " not found." << endl;
		}
	}

	void addDepartment(string schoolID, string deptName) {
		schoolNode* school = schoolTable->search(schoolID);
		if (school) {
			school->addDepartment(deptName);
		}
		else {
			cout << "School with ID " << schoolID << " not found." << endl;
		}
	}

	


	void insertIntoHashTable(schoolNode* school) {
		schoolTable->insert(school->schoolID, school);
	}

	void insertIntoMinHeap(schoolNode* school) {
		schoolHeap->insert(school);
	}

	void loadSchoolsFromFile(string fileName) {
		ifstream file(fileName);
		if (!file.is_open()) {
			cout << "Error: Could not open " << fileName << endl;
			return;
		}

		string line;
		getline(file, line); // Skip header

		while (getline(file, line)) {
			if (line.empty()) continue;

			size_t pos = 0;

			// Fixed columns
			string id = getNextField(line, pos);
			string name = getNextField(line, pos);
			string sector = getNextField(line, pos);
			string ratingStr = getNextField(line, pos);

			float rating = 0.0f, lat = 0.0f, lon = 0.0f;
			try { rating = stof(ratingStr); }
			catch (...) { rating = 0.0f; }

			string subjectsArr[10];
			int subCount = 0;

			bool coordinatesFound = false;

			
			while (pos < line.size()) {
				string segment = getNextField(line, pos);
				if (segment.empty()) continue;

				
				size_t q = segment.find('"');
				while (q != string::npos) {
					segment.erase(q, 1);
					q = segment.find('"');
				}
				
				while (!segment.empty() &&
					(segment[0] == ' ' || segment[0] == '\t'))
					segment.erase(segment.begin());

				
				bool isNumber = false;
				if (!segment.empty() &&
					(isdigit(static_cast<unsigned char>(segment[0])) || segment[0] == '-' || segment[0] == '+')) {
					// crude numeric check
					isNumber = (segment.find_first_not_of("0123456789+-.") == string::npos);
				}

				if (isNumber && !coordinatesFound) {
					
					try {
						lat = stof(segment);
						string lonStr = getNextField(line, pos);
						
						size_t q2 = lonStr.find('"');
						while (q2 != string::npos) {
							lonStr.erase(q2, 1);
							q2 = lonStr.find('"');
						}
						try { lon = stof(lonStr); }
						catch (...) {}
					}
					catch (...) {}
					coordinatesFound = true;
					break; 
				}
				else {
					
					if (subCount < 10)
						subjectsArr[subCount++] = segment;
				}
			}

			registerSchool(name, id, sector, rating, lat, lon, subjectsArr, subCount);
		}

		file.close();
		cout << "Education data loaded from " << fileName << endl;
	}


	void addSchool(string name, string id, string location, float rating, float lat, float lon, string* subjects, int subCount) {

		// 1. Create Node with Coordinates
		schoolNode* newSchool = new schoolNode(name, id, location, rating, lat, lon);
		for (int i = 0; i < subCount; i++) {
			newSchool->addSubject(subjects[i]);
			subjectTable->insert(subjects[i], newSchool);
		}

		if (!root) {
			root = newSchool;
		}
		else {
			schoolNode* temp = root;
			while (temp->next) {
				temp = temp->next;
			}
			temp->next = newSchool;
		}
		insertIntoHashTable(newSchool);
		insertIntoMinHeap(newSchool);

		//creating node for the mall first
		Node schoolGraphNode( id,  name , "SCHOOL", lat, lon);
		educationGraph->addNode(schoolGraphNode);
		educationGraph->connectToNearbyNodes(id, 2, "BUS_STOP");


		cout << "School " << name << " added to the education system & to city wide graph\n";
	}


	void registerSchool(string name, string id, string location, float rating, float lat, float lon, string* subjects, int subCount) {

		// 1. Create Node with Coordinates
		//schoolNode* newSchool = new schoolNode(name, id, location, rating, lat, lon);
		addSchool(name, id, location, rating,lat, lon,  subjects, subCount);
		cout << "School " << name << " added successfully with ID " << id << ".\n";
	}


	void loadDepartmentsFromFile(const string& fileName) {
		ifstream file(fileName);
		if (!file.is_open()) {
			cout << "Error: Could not open " << fileName << endl;
			return;
		}

		string line;
		getline(file, line); // skip header

		while (getline(file, line)) {
			if (line.empty()) continue;

			size_t pos = 0;
			string schoolID = getNextField(line, pos);
			string deptName = getNextField(line, pos);
			string deptIDStr = getNextField(line, pos);

			if (schoolID.empty() || deptName.empty()) continue;

			int deptID = 0;
			try { deptID = stoi(deptIDStr); }
			catch (...) {}

			schoolNode* school = schoolTable->search(schoolID);
			if (!school) {
				cout << "Warning: SchoolID " << schoolID
					<< " not found while loading departments.\n";
				continue;
			}

			school->addDepartment(deptName);
			departmentNode* dept = school->getDepartment(deptName);
			if (dept) dept->departmentID = deptID;
		}
		file.close();
		cout << "Departments loaded from " << fileName << endl;
	}


	void loadFacultyFromFile(const string& fileName) {
		ifstream file(fileName);
		if (!file.is_open()) {
			cout << "Error: Could not open " << fileName << endl;
			return;
		}

		string line;
		getline(file, line); // skip header

		while (getline(file, line)) {
			if (line.empty()) continue;

			size_t pos = 0;
			string schoolID = getNextField(line, pos);
			string deptName = getNextField(line, pos);
			string profName = getNextField(line, pos);
			string profIDStr = getNextField(line, pos); // unused, but parsed

			if (schoolID.empty() || deptName.empty() || profName.empty())
				continue;

			schoolNode* school = schoolTable->search(schoolID);
			if (!school) continue;

			departmentNode* dept = school->getDepartment(deptName);
			if (!dept) {
				school->addDepartment(deptName);
				dept = school->getDepartment(deptName);
			}

			dept->addFaculty(profName);
		}
		file.close();
		cout << "Faculty loaded from " << fileName << endl;
	}



	void loadClassesFromFile(const string& fileName) {
		ifstream file(fileName);
		if (!file.is_open()) {
			cout << "Error: Could not open " << fileName << endl;
			return;
		}

		string line;
		getline(file, line); // skip header

		while (getline(file, line)) {
			if (line.empty()) continue;

			size_t pos = 0;
			string schoolID = getNextField(line, pos);
			string deptName = getNextField(line, pos);
			string className = getNextField(line, pos);
			string classIDStr = getNextField(line, pos);

			if (schoolID.empty() || deptName.empty() || className.empty())
				continue;

			int classID = 0;
			try { classID = stoi(classIDStr); }
			catch (...) {}

			schoolNode* school = schoolTable->search(schoolID);
			if (!school) continue;

			departmentNode* dept = school->getDepartment(deptName);
			if (!dept) {
				school->addDepartment(deptName);
				dept = school->getDepartment(deptName);
			}

			dept->addClass(className, classID);
		}

		file.close();
		cout << "Classes loaded from " << fileName << endl;
	}


	
	

	void loadStudentsFromFile(const string& fileName) {
		ifstream file(fileName);
		if (!file.is_open()) {
			cout << "Error: Could not open " << fileName << endl;
			return;
		}

		string line;
		getline(file, line); // skip header

		while (getline(file, line)) {
			if (line.empty()) continue;

			size_t pos = 0;
			string schoolID = getNextField(line, pos);
			string deptName = getNextField(line, pos);
			string className = getNextField(line, pos);
			string studentName = getNextField(line, pos);
			string studentIDStr = getNextField(line, pos);

			if (schoolID.empty() || deptName.empty() || className.empty() || studentName.empty())
				continue;

			int studentID = 0;
			try { studentID = stoi(studentIDStr); }
			catch (...) {}

			schoolNode* school = schoolTable->search(schoolID);
			if (!school) continue;

			departmentNode* dept = school->getDepartment(deptName);
			if (!dept) {
				school->addDepartment(deptName);
				dept = school->getDepartment(deptName);
			}

			classNode* cls = dept->getClass(className);
			if (!cls) {
				dept->addClass(className);
				cls = dept->getClass(className);
			}

			cls->addStudent(studentName);  // or a version that uses studentID
		}
		file.close();
		cout << "Students loaded from " << fileName << endl;
	}



	void loadEducationData(const string& schoolsFile, const string& departmentsFile,const string& facultyFile, const string& classesFile, const string& studentsFile) {
		// 1) Schools (root nodes + subjects, coords, heap, hash)
		loadSchoolsFromFile(schoolsFile);

		loadDepartmentsFromFile(departmentsFile);

		// 3) Faculty for each department
		loadFacultyFromFile(facultyFile);

		// 4) Unique classes per department
		loadClassesFromFile(classesFile);

		// 5) Students for each class
		loadStudentsFromFile(studentsFile);
	}


	void registerDepartment(string schoolID, string deptName) {
		addDepartment(schoolID, deptName);
		cout << "Department " << deptName << " added successfully to school " << schoolID << ".\n";
	}


	void registerClass(string schoolID, string deptName, string className) {
		addClass(schoolID, deptName, className);
		cout << "Class " << className << " added successfully to department " << deptName << " in school " << schoolID << ".\n";
	}

	bool searchSchool(string id) {
		schoolNode* school = schoolTable->search(id);
		if (school)
		{
			cout << "School found: " << school->schoolName << ", ID: " << school->schoolID << ", Location: " << school->sector << ", Rating: " << school->rating << endl;
			return true;
		}
		else {
			cout << "School with ID " << id << " not found." << endl;
			return false;
		}
	}

	void performDepartmentSearch(string schoolID, string deptName) {
		schoolNode* s = schoolTable->search(schoolID);
		if (!s) { cout << "[!] School " << schoolID << " not found." << endl; return; }

		// Uses your new DepartmentHashTable for O(1) lookup
		departmentNode* d = s->getDepartment(deptName);
		if (d) {
			cout << "\n--- Department Found ---" << endl;
			cout << "Name: " << d->departmentName << " (ID: " << d->departmentID << ")" << endl;
			cout << "Faculty Count: " << d->facultyCount << endl;
			cout << "Class Count: " << d->classCount << endl;
			d->displayFaculty(); // Show faculty list
		}
		else {
			cout << "[!] Department " << deptName << " not found in " << s->schoolName << "." << endl;
		}
	}

	void performClassSearch(string schoolID, string deptName, string className) {
		schoolNode* s = schoolTable->search(schoolID);
		if (!s) { cout << "[!] School " << schoolID << " not found." << endl; return; }

		departmentNode* d = s->getDepartment(deptName);
		if (!d) { cout << "[!] Department " << deptName << " not found." << endl; return; }

		// Uses your new ClassHashTable for O(1) lookup
		classNode* c = d->getClass(className);
		if (c) {
			cout << "\n--- Class Found ---" << endl;
			cout << "Name: " << c->className << " (ID: " << c->classID << ")" << endl;
			cout << "Students Enrolled: " << c->studentCount << endl;
			c->displayStudents(); // Show student list
		}
		else {
			cout << "[!] Class " << className << " not found." << endl;
		}
	}

	void performFacultySearch(string schoolID, string deptName, string facultyName) {
		schoolNode* s = schoolTable->search(schoolID);
		if (!s) { cout << "[!] School " << schoolID << " not found." << endl; return; }

		departmentNode* d = s->getDepartment(deptName);
		if (!d) { cout << "[!] Department " << deptName << " not found." << endl; return; }

	
		if (d->searchFaculty(facultyName)) {
		
		}
		else {
			cout << "[!] Faculty member \"" << facultyName << "\" not found." << endl;
		}
	}

	void performStudentSearch(string schoolID, string deptName, string className, string studentName) {
		schoolNode* s = schoolTable->search(schoolID);
		if (!s) { cout << "[!] School " << schoolID << " not found." << endl; return; }

		departmentNode* d = s->getDepartment(deptName);
		if (!d) { cout << "[!] Department " << deptName << " not found." << endl; return; }

		classNode* c = d->getClass(className);
		if (!c) { cout << "[!] Class " << className << " not found." << endl; return; }

		if (c->searchStudentName(studentName)) {
			cout << "[Success] Student \"" << studentName << "\" is enrolled in " << className << "." << endl;
		}
		else {
			cout << "[!] Student \"" << studentName << "\" not found in this class." << endl;
		}
	}

	void display() {
		schoolNode* temp = root;
		while (temp) {
			temp->displayHierarchy(); // Calls the recursive display 
			temp = temp->next;
			cout << "----------------------" << endl;
		}
	}

	void showLowRatedSchool() {
		schoolNode * temp = schoolHeap->getMin();
		if (temp) {
			temp->displayHierarchy();
		}
		else {
			cout << "No schools available." << endl;
		}
	}

	void showTopSchool() {
		schoolNode* temp = schoolHeap->getMax();
		if (temp) {
			temp->displayHierarchy();
		}
		else {
			cout << "No schools available." << endl;
		}
	}

	
	void searchSubject(string subjectName) {
		subjectTable->displaySchoolsForSubject(subjectName);
	}

	

	double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
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


	

	void findNearestSchool (double userLat, double userLon) {
		cout << "\n ========== Nearest School Search ==========" << endl;

		string nearestSchool = educationGraph->findNearestNode(userLat, userLon, "SCHOOL");

		if (nearestSchool.empty()) {
			cout << "[!] No schools found in the network.\n";
			return;
		}

		schoolNode* school = schoolTable->search(nearestSchool);
		if (!school) {
						cout << "[!] School with ID " << nearestSchool << " not found in the education system.\n";
			return;
		}
		double distance = calculateDistance(userLat, userLon, school->getLatitude(), school->getLongitude());

		cout << "Nearest School: " << school->schoolName << " (ID: " << school->schoolID << ")\n";
		cout << "Location: " << school->sector << "\n";
		cout << "Distance: " << distance << " km\n";
	}


	void findNearestSchoolBySubject(double userLat, double userLon, string subject) {
		cout << "\n========== Nearest School Search (By Subject) ==========" << endl;
		cout << "Looking for schools offering: " << subject << "..." << endl;

		int count = 0;
		schoolNode** candidates = subjectTable->search(subject, count);

		if (candidates == nullptr || count == 0) {
			cout << "[!] No schools found offering \"" << subject << "\"." << endl;
			return;
		}

		
		schoolNode* nearestSchool = nullptr;
		double minDistance = 9999999.0; // Start with a huge number

		for (int i = 0; i < count; i++) {
			schoolNode* s = candidates[i];

	
			double dist = calculateDistance(userLat, userLon, s->getLatitude(), s->getLongitude());

			if (dist < minDistance) {
				minDistance = dist;
				nearestSchool = s;
			}
		}

		if (nearestSchool != nullptr) {
			cout << "Found " << count << " schools. The nearest is:" << endl;
			cout << " -> Name: " << nearestSchool->schoolName << endl;
			cout << " -> ID:   " << nearestSchool->schoolID << endl;
			cout << " -> Dist: " << minDistance << " km" << endl;

			// Suggest finding a route
			cout << "\n[Tip] You can find a route to this school using the Graph menu!" << endl;
		}

	
		delete[] candidates;
	}

	void findRouteToSchool(string fromNodeid, string toSchoolID) {
		cout << "\n ========== Route Finding ==========" << endl;

		schoolNode* school = schoolTable->search(toSchoolID);
		if (!school) {
			cout << "[!] School with ID " << toSchoolID << " not found in the education system.\n";
			return;
		}

		cout << "Finding route to School: " << school->schoolName << " (location: " << school->sector << ")\n";
		// use dijkstra with no specific filter which basically means we can go through any type of nodes 
		educationGraph->dijkstraAlgo(fromNodeid, toSchoolID, "ANY");


	}


	void displayNetwork(){
		cout << endl << "========== Education Sector Graph ==========" << endl;
		educationGraph->displayGraph("SCHOOL");
	}

	// inside educationSystem class (public or private)
	void findNearestSchoolFromNode() {
		if (!educationGraph) {
			cout << "[!] City graph not available.\n";
			return;
		}

		cout << "\n=== Find Nearest School (from any node) ===" << endl;
		cout << "Enter your current node ID (e.g., S01, Stop1, H-G-10-180, M01): ";

		string currentID;
		getline(cin, currentID);

		if (!educationGraph->hasNode(currentID)) {
			cout << "[!] Node '" << currentID << "' not found in city graph.\n";
			return;
		}

		Node* curNode = educationGraph->getNode(currentID);
		double lat = curNode->getLatitude();
		double lon = curNode->getLongitude();

		string nearestSchoolID = educationGraph->findNearestNode(lat, lon, "SCHOOL");
		if (nearestSchoolID.empty()) {
			cout << "No school nodes found in the graph.\n";
			return;
		}

		Node* schoolNode = educationGraph->getNode(nearestSchoolID);
		cout << "\nNearest school to " << currentID << " is:" << endl;
		cout << "  ID   : " << nearestSchoolID << endl;
		cout << "  Name : " << (schoolNode ? schoolNode->getName() : "(unknown)") << endl;
		cout << "  Coord: (" << schoolNode->getLatitude()
			<< ", " << schoolNode->getLongitude() << ")\n";

		// Optional: show shortest path from current node to that school
		cout << "\nFinding shortest path from " << currentID
			<< " to " << nearestSchoolID << " ..." << endl;
		educationGraph->dijkstraAlgo(currentID, nearestSchoolID, "ANY");
	}


	


	// --- INTEGRATED MENU SYSTEM ---

	void displayMenu() {
		cout << endl << "========================================" << endl;
		cout << "      SMART CITY EDUCATION SYSTEM" << endl;
		cout << "========================================" << endl;
		cout << "1.  Register New School" << endl;
		cout << "2.  Add Department to School" << endl;
		cout << "3.  Add Faculty to Department" << endl;
		cout << "4.  Add Class to Department" << endl;
		cout << "5.  Add Student to Class" << endl;
		cout << "6.  Add Subject to School" << endl;          // NEW

		cout << "\n--- SEARCH & DISPLAY ---" << endl;
		cout << "7.  Display All Schools (Hierarchy)" << endl;
		cout << "8.  Display School Network (Graph View)" << endl;
		cout << "9.  Find Nearest School from Node ID" << endl;
		cout << "10. Search School (by ID)" << endl;
		cout << "11. Search Department (In specific School)" << endl;
		cout << "12. Search Class (In specific Department of School)" << endl;
		cout << "13. Search Faculty Member (In specific Department)" << endl;
		cout << "14. Search Student (In class of specific Department of School)" << endl;
		cout << "15. Search Schools by Subject" << endl;

		cout << "\n--- SMART FEATURES ---" << endl;
		cout << "16. Find Nearest School (Location Based)" << endl;
		cout << "17. Find Nearest School by Subject" << endl;
		cout << "18. Find Route to School" << endl;
		cout << "19. Show Top Ranked Schools" << endl;
		cout << "20. Show Lowest Rated School" << endl;              // NEW
		cout << "21. Show Schools in Ascending Order" << endl;        // NEW

		cout << "0.  Exit" << endl;
		cout << "========================================" << endl;
		cout << "Enter choice: ";
	}


	void run() {
		int choice;
		while (true) {
			displayMenu();
			if (!(cin >> choice)) {
				cin.clear();
				cin.ignore(10000, '\n');
				continue;
			}
			cin.ignore(); // remove newline after number

			if (choice == 0) {
				cout << "Exiting Education System..." << endl;
				break;
			}

			switch (choice) {
			case 1: { // Register School
				string name, id, location, subjectsStr;
				float rating, lat, lon;
				cout << "Enter School Name: "; getline(cin, name);
				cout << "Enter School ID: "; getline(cin, id);
				cout << "Enter Location (Sector): "; getline(cin, location);
				cout << "Enter Rating (0-5): "; cin >> rating;
				cout << "Enter Latitude: "; cin >> lat;
				cout << "Enter Longitude: "; cin >> lon;
				cin.ignore();

				cout << "Enter Subjects (comma separated): ";
				getline(cin, subjectsStr);

				string subjects[10];
				int subCount = 0;
				size_t pos = 0;

				while (pos < subjectsStr.size() && subCount < 10) {
					string token = getNextField(subjectsStr, pos);
					if (!token.empty())
						subjects[subCount++] = token;
				}

				registerSchool(name, id, location, rating, lat, lon, subjects, subCount);
				break;
			}

			case 2: { // Add Department
				string id, dept;
				cout << "School ID: "; getline(cin, id);
				cout << "Dept Name: "; getline(cin, dept);
				registerDepartment(id, dept);
				break;
			}

			case 3: { // Add Faculty
				string id, dept, fac;
				cout << "School ID: "; getline(cin, id);
				cout << "Dept Name: "; getline(cin, dept);
				cout << "Faculty Name: "; getline(cin, fac);

				schoolNode* s = schoolTable->search(id);
				if (s) {
					departmentNode* d = s->getDepartment(dept);
					if (d) {
						d->addFaculty(fac);
						cout << "Faculty member added.\n";
					}
					else {
						cout << "Department not found.\n";
					}
				}
				else {
					cout << "School not found.\n";
				}
				break;
			}

			case 4: { // Add Class
				string id, dept, cls;
				cout << "School ID: "; getline(cin, id);
				cout << "Dept Name: "; getline(cin, dept);
				cout << "Class Name: "; getline(cin, cls);
				registerClass(id, dept, cls);
				break;
			}

			case 5: { // Add Student
				string id, dept, cls, stu;
				cout << "School ID: "; getline(cin, id);
				cout << "Dept Name: "; getline(cin, dept);
				cout << "Class Name: "; getline(cin, cls);
				cout << "Student Name: "; getline(cin, stu);
				addStudent(id, dept, cls, stu);
				break;
			}

			case 6: { // Add Subject to School (NEW)
				string id, sub;
				cout << "\n=== Add Subject to School ===\n";
				cout << "School ID (e.g., S01): ";
				getline(cin, id);
				cout << "Subject to add (e.g., Data Science): ";
				getline(cin, sub);

				schoolNode* s = schoolTable->search(id);
				if (!s) {
					cout << "School not found.\n";
				}
				else {
					bool inserted = false;
					// avoid duplicate and find empty slot in subjects array
					for (int i = 0; i < 10; ++i) {
						if (s->subjects[i] == sub) {
							cout << "Subject already exists for this school.\n";
							inserted = true;
							break;
						}
					}
					if (!inserted) {
						for (int i = 0; i < 10; ++i) {
							if (s->subjects[i].empty()) {
								s->subjects[i] = sub;
								cout << "Subject added.\n";
								inserted = true;
								break;
							}
						}
						if (!inserted) {
							cout << "Cannot add more subjects (list is full).\n";
						}
					}
				}
				break;
			}

			case 7:
				display();   // all schools with hierarchy
				break;

			case 8:
				displayNetwork();     // school graph view
				break;

			case 9:
				findNearestSchoolFromNode();
				break;

			case 10: { // Search School (by ID)
				cout << "\n=== Search School (by ID) ===\n";
				cout << "Format example: S01, S02, S03 ...\n";
				string id = readNonEmpty("Enter School ID: ");
				searchSchool(id);
				break;
			}

			case 11: { // Search Department
				cout << "\n=== Search Department ===\n";
				cout << "Expected: existing School ID and exact Dept name.\n";
				string id = readNonEmpty("Enter School ID: ");
				string dept = readNonEmpty("Enter Department Name (e.g., Computer Science): ");
				performDepartmentSearch(id, dept);
				break;
			}

			case 12: { // Search Class
				cout << "\n=== Search Class ===\n";
				cout << "Expected: School ID, Department name, Class name.\n";
				string id = readNonEmpty("Enter School ID: ");
				string dept = readNonEmpty("Enter Department Name: ");
				string cls = readNonEmpty("Enter Class Name: ");
				performClassSearch(id, dept, cls);
				break;
			}

			case 13: { // Search Faculty
				cout << "\n=== Search Faculty Member ===\n";
				cout << "Expected: School ID, Department name, Faculty full name.\n";
				string id = readNonEmpty("Enter School ID: ");
				string dept = readNonEmpty("Enter Department Name: ");
				string fac = readNonEmpty("Enter Faculty Name: ");
				performFacultySearch(id, dept, fac);
				break;
			}

			case 14: { // Search Student
				cout << "\n=== Search Student ===\n";
				cout << "Expected: School ID, Department, Class, Student name.\n";
				string id = readNonEmpty("Enter School ID: ");
				string dept = readNonEmpty("Enter Department Name: ");
				string cls = readNonEmpty("Enter Class Name: ");
				string stu = readNonEmpty("Enter Student Name: ");
				performStudentSearch(id, dept, cls, stu);
				break;
			}

			case 15: { // Search Schools by Subject
				cout << "\n=== Search Schools by Subject ===\n";
				cout << "Example subjects: Math, CS, Physics, AI, Islamiat.\n";
				string sub = readNonEmpty("Enter Subject: ");
				searchSubject(sub);
				break;
			}

			case 16: { // Find nearest school (lat/lon)
				double lat, lon;
				cout << "Lat: "; cin >> lat;
				cout << "Lon: "; cin >> lon;
				cin.ignore();
				findNearestSchool(lat, lon);
				break;
			}

			case 17: { // Find nearest school by subject
				string sub;
				double lat, lon;
				cout << "Subject: "; getline(cin, sub);
				cout << "Lat: "; cin >> lat;
				cout << "Lon: "; cin >> lon;
				cin.ignore();
				findNearestSchoolBySubject(lat, lon, sub);
				break;
			}

			case 18: { // Find route to a school
				string from, to;
				cout << "From Node (e.g., house / stop ID): ";
				getline(cin, from);
				cout << "To School ID: ";
				getline(cin, to);
				findRouteToSchool(from, to);
				break;
			}

			case 19:
				showTopSchool();              // top-ranked using heap
				break;

			case 20:
				schoolHeap->printLowestSchool();      // uses min-heap
				break;

			case 21:
				schoolHeap->printSchoolsAscending(); // uses min-heap to print all
				break;

			default:
				cout << "Invalid choice.\n";
			}
		}
	}



};