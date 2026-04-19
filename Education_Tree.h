#pragma once
#include "departmentHash.h"
#include "classHash.h"
#include <iostream>
#include <string>
#include <cstdlib>  // for rand
#include <ctime> 
using namespace std;


struct studentNode { //  linked list of students in class
	string name;
	int id;
	float gpa;
	studentNode* next;
	studentNode(string n, int i) : name(n), id(i), next(nullptr) {
		float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); 
		gpa = r * 4.0f; // 0..4
	
	}

	studentNode() {
		this->gpa = 0.0;
		this->id = 0;		
		this->name = "";
		this->next = nullptr;

	}

};

struct facultyNode {
	string facultyName;
	int facultyID;
	facultyNode* next;
	facultyNode( string fn = "" ,int id = 0 ) : facultyName(fn), facultyID(id), next(nullptr) {}
	facultyNode(){
		this->facultyName = "";
		this->facultyID = 0;
		this->next = nullptr;
	}


};

//level 3 classes 
struct classNode {
	string className;
	int classID;
	studentNode* studentsHead; // linked list of students in class
	classNode* next;
	int studentCount;
	classNode(string cc = "", int id = 0 ) : className(cc), classID(id), studentsHead(nullptr), next(nullptr) {
		
		this->studentCount = 1;
	}
	

	void addStudent(string studentName) {
		studentNode* newStudent = new studentNode(studentName,studentCount++ );
		if (!studentsHead) {
			studentsHead = newStudent;
		}
		else {
			studentNode* temp = studentsHead;
			while (temp->next) {
				temp = temp->next;
			}
			temp->next = newStudent;
		}
	}

	bool removeStudent(int id) {
		if (!studentsHead) return false;
		if (studentsHead->id == id) {
			studentNode* toDelete = studentsHead;
			studentsHead = studentsHead->next;
			delete toDelete;
			return true;
		}
		studentNode* temp = studentsHead;
		studentNode * prev = nullptr;
		while (temp->next && temp->id != id) {
			prev  = temp;
			temp = temp->next;

		}
		
		if (temp->id == id) {
			prev->next = temp->next;
			delete temp;
			return true;
		}
		return false;
	
	}

	void displayStudents() {
		if (studentsHead == nullptr) {
			cout << "\t\t   (No students registered)" << endl;
			return;
		}

		cout << "\t\t   [Students]:" << endl;
		studentNode* temp = studentsHead;
		while (temp) {
			cout << "\t\t    -> " << temp->name
				<< " (ID: " << temp->id
				<< ", GPA: " << temp->gpa << ")" << endl;
			temp = temp->next;
		}
		cout << endl; // Extra space for cleanliness
	}


	bool searchStudentID(int id) {
		studentNode* temp = studentsHead;
		while (temp) {
			if (temp->id == id) {
				return true;
			}
			temp = temp->next;
		}
		return false;

	}

	bool searchStudentName(string name) {
		studentNode* temp = studentsHead;
		while (temp) {
			if (temp->name == name) {
				return true;
			}
			temp = temp->next;
		}
		return false;
	}

	~classNode() {
		studentNode* current = studentsHead;
		studentNode* nextStudent; 
		while (current) {
			nextStudent= current->next;
			delete current;
			current = nextStudent;
		}
	}
};


//level 2 departments

struct departmentNode {
	string departmentName;
	int departmentID;

	departmentNode* next;
	classNode* firstClass; // child of department  
	int classCount;
	int facultyCount;
	facultyNode * firstFaculty; // linked list of faculty in department
	ClassHashTable classTable;
	 
	departmentNode(string dn = "", int di = 0) : departmentName(dn), departmentID(di), next(nullptr), firstClass(nullptr), classCount(0) {
		this->facultyCount = 1;
	}


	void addFaculty(string facultyName) {
		facultyNode * temp = new facultyNode(facultyName, facultyCount++);
		if (firstClass == nullptr) {
			firstFaculty = temp;
			return;
		}	
		facultyNode* curr = firstFaculty;		
		while (curr->next) {
			curr = curr->next;
		}
		curr->next = temp;
		return;
	}

	bool deleteFaculty(string facultyName) {
		if (!firstFaculty) return false;
		if (firstFaculty->facultyName == facultyName) {
			facultyNode* toDelete = firstFaculty;
			firstFaculty = firstFaculty->next;
			delete toDelete;
			return true;
		}
		facultyNode* temp = firstFaculty;
		facultyNode* prev = nullptr;
		while (temp->next && temp->facultyName != facultyName) {
			prev = temp;
			temp = temp->next;
		}
		if (temp->facultyName == facultyName) {
			prev->next = temp->next;
			delete temp;
			return true;
		}
		return false;
	}

	bool searchFaculty (string facultyName) {
		facultyNode* temp = firstFaculty;
		while (temp) {
			if (temp->facultyName == facultyName) {
				cout << "Faculty Found: " << temp->facultyName << " (ID: " << temp->facultyID << ")" << endl;
				return 1;
			}
			temp = temp->next;
		}
		return 0;
	}

	void displayFaculty() {
		if (firstFaculty == nullptr) {
			cout << "\t\t   (No faculty registered)" << endl;
			return;
		}

		cout << "\t\t   [Faculty Members]:" << endl;
		facultyNode* temp = firstFaculty;
		while (temp) {
			
			cout << "\t\t    -> " << temp->facultyName
				<< " (ID: " << temp->facultyID << ")" << endl;
			temp = temp->next;
		}
		cout << endl; // Extra space for cleanliness
	}
	void addClass(const string& className, int classID = -1) {
		
		classNode* existing = classTable.search(className);
		if (existing) {
			if (classID != -1) existing->classID = classID; // update from file
			return;
		}

		if (classID == -1) classID = classCount;
		classNode* newClass = new classNode(className, classID);

		if (!firstClass) firstClass = newClass;
		else {
			classNode* temp = firstClass;
			while (temp->next) temp = temp->next;
			temp->next = newClass;
		}

		classTable.insert(className, newClass);
		if (classID >= classCount) classCount = classID + 1;
	}

	bool deleteClass(const string& className) {
		if (!firstClass) return false;

		if (firstClass->className == className) {
			classNode* toDelete = firstClass;
			firstClass = firstClass->next;
			classTable.deleteKey(className);
			delete toDelete;
			return true;
		}

		classNode* temp = firstClass;
		classNode* prev = nullptr;
		while (temp && temp->className != className) {
			prev = temp;
			temp = temp->next;
		}
		if (!temp) return false;

		prev->next = temp->next;
		classTable.deleteKey(className);
		delete temp;
		return true;
	}


	classNode* getClass(const string& className) {
		return classTable.search(className);
	}

	void searchClass(const string& className) {
		classNode* c = classTable.search(className);
		if (c) {
			cout << "Class Found: " << c->className
				<< " (ID: " << c->classID << ")" << endl;
		}
		else {
			cout << "Class with Name " << className << " not found." << endl;
		}
	}



	void displayClasses() {
		if (firstClass == NULL) {
			cout << "\t\t   (No classes available)" << endl;
			return;
		}

		classNode* temp = firstClass;
		while (temp) {
			// Uses ASCII characters to make a tree branch "L" shape
			cout << "\t\t|__ [" << temp->classID << "] " << temp->className << endl;

			// to show the Instructor name too
			// cout << "\t\t|    (Instructor: " << temp->instructor << ")" << endl;
			temp->displayStudents();
			temp = temp->next;
		}
		cout << "\t\t|" << endl; // Empty filler line for spacing
	}



	~departmentNode() {
		classNode* current = firstClass;
		classNode* nextClass;
		while (current) {
			nextClass = current->next;
			delete current;
			current = nextClass;
		}
	}


};

//level 1 school
struct schoolNode {
	string schoolName;
	string schoolID;
	string sector; // location
	float rating;
	string* subjects; // array of subjects offered
	schoolNode* next;
	int departmentCount;
	float coordinates[2]; // latitude and longitude
	DepartmentHashTable departmentTable;

	departmentNode* firstDepartment; // child of school
	schoolNode(string sn = "", string si = "", string sec = "", float r = 0 , float lat = 0.0, float lon = 0.0) : schoolName(sn), schoolID(si), sector(sec), rating(r), next(nullptr), firstDepartment(nullptr) {
		subjects = new string[5]; // assuming max 5 subjects for simplicity
		this->departmentCount = 0;
		//departmentTable = new DepartmentHashTable;
		coordinates[0] = lat;
		coordinates[1] = lon;
	}

	schoolNode(){
		this->schoolName = "";
		this->schoolID = "";
		this->sector = "";
		this->rating = 0;
		this->next = nullptr;
		this->firstDepartment = nullptr;
		this->subjects = new string[10];
		this->departmentCount = 0;
		coordinates[0] = 0.0;
		coordinates[1] = 0.0;
	//	departmentTable = new DepartmentHashTable;
	}

	~schoolNode() {
		departmentNode* current = firstDepartment;
		departmentNode* nextDepartment;
		while (current) {
			nextDepartment = current->next;
			delete current;
			current = nextDepartment;
		}
		delete[] subjects;
		
	}

	bool addSubject(const string& sub) {
		
		for (int i = 0; i < 10; ++i) {
			if (subjects[i] == sub) {
				cout << "Subject already exists for this school.\n";
				return false;
			}
		}
	
		for (int i = 0; i < 10; ++i) {
			if (subjects[i].empty()) {
				subjects[i] = sub;
				return true;
			}
		}
		cout << "Cannot add more subjects (subject list is full).\n";
		return false;
	}


	bool hasSubject(string querySubject) {
		for (int i = 0; i < 5; i++) {
			// Simple string matching
			if (subjects[i] == querySubject) {
				return true;
			}
		}
		return false;
	}

	void addStudent(string departmentName, string className, string studentName, int studentID) {
		departmentNode* temp = departmentTable.search(departmentName);
		if (temp) {
			classNode* classTemp = temp->firstClass;
			while (classTemp) {
				if (classTemp->className == className) {
					classTemp->addStudent(studentName);
					return;
				}
				classTemp = classTemp->next;
			}
			cout << "Class " << className << " not found in Department " << departmentName << "." << endl;
			return;
		}

		cout << "Department " << departmentName << " not found." << endl;
	}

	void addClass (string departmentName, string classID = "") {
		departmentNode* temp = departmentTable.search(departmentName);
		if (temp) {
			temp->addClass(classID);
			return;
		}

		cout << "Department " << departmentName << " not found." << endl;
	}

	void addDepartment(string departmentName) {
		departmentNode * newDepartment =  new departmentNode(departmentName, this->departmentCount++ );
		departmentTable.insert(departmentName, newDepartment);
		if (!firstDepartment)
			firstDepartment = newDepartment;
		else {
			departmentNode* temp = firstDepartment;
			while (temp->next) {
				temp = temp->next;
			}
			temp->next = newDepartment;
		}
	}

	

	bool searchDepartment (string departmentName) {
		departmentNode* temp = departmentTable.search(departmentName);
		if (temp) {
			cout << "Department Found: " << temp->departmentName << " (ID: " << temp->departmentID << ")" << endl;
			return 1;
		}
		return 0;
	}

	departmentNode* getDepartment(const string& departmentName) {
		return departmentTable.search(departmentName);
	}

	bool deleteDepartment(string departmentName) {
		//departmentNode * temp = getDepartment(departmentName);
		
		if (!firstDepartment) return false;

		if (firstDepartment->departmentName == departmentName) {
			departmentNode* toDelete = firstDepartment;
			firstDepartment = firstDepartment->next;
			delete toDelete;
			return true;
		}

		departmentNode* temp = firstDepartment;
		departmentNode* prev = nullptr;
		while (temp->next && temp->departmentName != departmentName) {
			prev = temp;
			temp = temp->next;
		}
		if (temp->departmentName == departmentName) {
			prev->next = temp->next;
			departmentTable.deleteKey(departmentName);
			delete temp;
			return true;
		}
		return false;		
	}

	void displayHierarchy() {
		cout << "School: " << schoolName << " (Rating: " << rating << ")" << endl;
		cout << "   Location: " << sector << " [" << coordinates[0] << ", " << coordinates[1] << "]" << endl;

		// Print Subjects
		cout << "   Subjects: [ ";
		for (int i = 0; i < 5; i++) {
			if (subjects[i] != "") cout << subjects[i] << " ";
		}
		cout << "]" << endl;

		if (firstDepartment == NULL) {
			cout << "\t(No departments)" << endl;
			return;
		}

		departmentNode* temp = firstDepartment;
		while (temp) {
			cout << "\t|__ [Dept] " << temp->departmentName << endl;

			// Show faculty for this department
			temp->displayFaculty();

			// Show classes for this department
			temp->displayClasses();

			temp = temp->next;
		}
	}

	double getLatitude() {
		return coordinates[0];
	}
	double getLongitude() {
		return coordinates[1];
	}
	

};