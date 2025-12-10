#ifndef EDUCATIONSECTOR_H
#define EDUCATIONSECTOR_H

#include "Tree.h"
#include "HashTable.h"
#include "MinHeap.h"
#include "Utils.h"
#include <iostream>
using namespace std;

// student structure
struct Student {
    char studentID[50];
    char name[100];
    char cnic[20];        // Link to population
    char schoolID[50];    // Which school
    int age;
    char className[50];
    
    Student() {
        studentID[0] = '\0';
        name[0] = '\0';
        cnic[0] = '\0';
        schoolID[0] = '\0';
        age = 0;
        className[0] = '\0';
    }
};

// faculty structure
struct Faculty {
    char facultyID[50];
    char name[100];
    char department[100];
    char subject[100];
    
    Faculty() {
        facultyID[0] = '\0';
        name[0] = '\0';
        department[0] = '\0';
        subject[0] = '\0';
    }
};

// school structure
struct School {
    char schoolID[50];
    char name[100];
    char sector[50];
    double rating;
    char subjects[200];
    Tree* organizationTree;  
    
    School() {
        schoolID[0] = '\0';
        name[0] = '\0';
        sector[0] = '\0';
        rating = 0.0;
        subjects[0] = '\0';
        organizationTree = new Tree();
    }
    
    ~School() {
        delete organizationTree;
    }
};

// education system class
class EducationSystem {
private:
    HashTable* schoolTable;
    HashTable* facultyTable;
    HashTable* studentTable;
    MinHeap* schoolRanking;
    int schoolCount;
    int facultyCount;
    int studentCount;

public:
    EducationSystem() {
        schoolTable = new HashTable(50);
        facultyTable = new HashTable(100);
        studentTable = new HashTable(500);
        schoolRanking = new MinHeap(50);
        schoolCount = 0;
        facultyCount = 0;
        studentCount = 0;
    }
    
    // registering school
    void registerSchool(const char* schoolID, const char* name, const char* sector, 
                       double rating, const char* subjects) {
        School* school = new School();
        stringCopyN(school->schoolID, schoolID, 50);
        stringCopyN(school->name, name, 100);
        stringCopyN(school->sector, sector, 50);
        school->rating = rating;
        stringCopyN(school->subjects, subjects, 200);
        
        // initialize school tree with root
        school->organizationTree->setRoot(0, name);
        
        schoolTable->insert(schoolID, school);
        
        // add to ranking (using negative rating for min heap)
        schoolTable->insert(schoolID, school);
        schoolCount++;
        
        // School registered (quiet mode)
    }
    
    // grabbing stats
    int getSchoolCount() { return schoolCount; }
    int getFacultyCount() { return facultyCount; }
    int getStudentCount() { return studentCount; }
    
    // Helper: Check if school exists (for cross-sector validation)
    bool schoolExists(const char* schoolID) {
        return schoolTable->contains(schoolID);
    }
    
    // add department to school
    void addDepartment(const char* schoolID, const char* deptName) {
        School* school = (School*)schoolTable->search(schoolID);
        if (school == nullptr) {
            cout << "School not found!" << endl;
            return;
        }
        
        school->organizationTree->addNode(school->name, 0, deptName);
        // cout << "Added department: " << deptName << " to " << school->name << endl;
    }
    
    // add class to department
    void addClass(const char* schoolID, const char* deptName, const char* className) {
        School* school = (School*)schoolTable->search(schoolID);
        if (school == nullptr) {
            cout << "School not found!" << endl;
            return;
        }
        
        school->organizationTree->addNode(deptName, 0, className);
        // cout << "Added class: " << className << " to department " << deptName << endl;
    }
    
    // register faculty
    void registerFaculty(const char* facultyID, const char* name, 
                        const char* department, const char* subject) {
        Faculty* faculty = new Faculty();
        stringCopyN(faculty->facultyID, facultyID, 50);
        stringCopyN(faculty->name, name, 100);
        stringCopyN(faculty->department, department, 100);
        stringCopyN(faculty->subject, subject, 100);
        
        facultyTable->insert(facultyID, faculty);
        facultyCount++;
        cout << "Registered faculty: " << name << endl;
    }
    
    // register student
    void registerStudent(const char* studentID, const char* name, const char* cnic,
                        const char* schoolID, int age, const char* className) {
        Student* student = new Student();
        stringCopyN(student->studentID, studentID, 50);
        stringCopyN(student->name, name, 100);
        stringCopyN(student->cnic, cnic, 20);
        stringCopyN(student->schoolID, schoolID, 50);
        student->age = age;
        stringCopyN(student->className, className, 50);
        
        studentTable->insert(studentID, student);
        studentCount++;
        cout << "Registered student: " << name << endl;
    }
    
    // display school details
    void displaySchool(const char* schoolID) {
        School* school = (School*)schoolTable->search(schoolID);
        if (school == nullptr) {
            cout << "School not found!" << endl;
            return;
        }
        
        cout << "\n--- School Details ---" << endl;
        cout << "School ID: " << school->schoolID << endl;
        cout << "Name: " << school->name << endl;
        cout << "Sector: " << school->sector << endl;
        cout << "Rating: " << school->rating << endl;
        cout << "Subjects: " << school->subjects << endl;
        
        cout << "\nOrganization Structure:" << endl;
        school->organizationTree->display();
    }
    
    // Display all schools summary - NOW PROPERLY ITERATES HASHTABLE
    void displayAllSchools() {
        cout << "\n========== ALL SCHOOLS SUMMARY ==========" << endl;
        cout << "Total Schools: " << schoolCount << endl;
        cout << "Total Faculty: " << facultyCount << endl;
        cout << "Total Students: " << studentCount << endl;
        
        char** schoolKeys = new char*[100];
        int count;
        schoolTable->getAllKeys(schoolKeys, &count);
        
        if (count == 0) {
            cout << "\n[INFO] No schools registered yet." << endl;
        } else {
            cout << "\nRegistered Schools: ";
            for (int i = 0; i < count; i++) {
                cout << schoolKeys[i];
                if (i < count - 1) cout << ", ";
            }
            cout << endl;
            cout << "Use 'Display school details' to see individual school info" << endl;
        }
        
        delete[] schoolKeys;
        cout << "==========================================" << endl;
    }
    
    // Display school rankings (sorted by rating)
    void displaySchoolRankings() {
        cout << "\n========== SCHOOL RANKINGS ==========" << endl;
        
        char** schoolKeys = new char*[100];
        int count;
        schoolTable->getAllKeys(schoolKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No schools registered yet." << endl;
            delete[] schoolKeys;
            return;
        }
        
        // Simple bubble sort by rating (descending)
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                School* school1 = (School*)schoolTable->search(schoolKeys[j]);
                School* school2 = (School*)schoolTable->search(schoolKeys[j + 1]);
                
                if (school1 && school2 && school1->rating < school2->rating) {
                    // Swap keys
                    char* temp = schoolKeys[j];
                    schoolKeys[j] = schoolKeys[j + 1];
                    schoolKeys[j + 1] = temp;
                }
            }
        }
        
        // Display sorted schools
        cout << "Rank | School Name              | Sector | Rating" << endl;
        cout << "-----+-------------------------+--------+--------" << endl;
        
        for (int i = 0; i < count; i++) {
            School* school = (School*)schoolTable->search(schoolKeys[i]);
            if (school != nullptr) {
                cout << " " << (i + 1) << "   | ";
                cout << school->name;
                
                // Padding for name (25 chars)
                int nameLen = stringLength(school->name);
                for (int k = nameLen; k < 24; k++) cout << " ";
                
                cout << " | " << school->sector;
                
                // Padding for sector (6 chars)
                int sectorLen = stringLength(school->sector);
                for (int k = sectorLen; k < 6; k++) cout << " ";
                
                cout << " | " << school->rating << "/5.0" << endl;
            }
        }
        
        delete[] schoolKeys;
        cout << "=====================================" << endl;
    }
    
    // ========== MANUAL REGISTRATION METHODS ==========
    
    // Register school manually
    void registerSchoolManual() {
        char schoolID[50], name[100], sector[50], subjects[200];
        double rating;
        
        cout << "\n=== Register New School ===" << endl;
        
        // Generate unique ID
        generateUniqueID("SCH", schoolID);
        cout << "Generated School ID: " << schoolID << endl;
        
        cout << "Enter school name: ";
        cin.ignore();
        cin.getline(name, 100);
        
        cout << "Enter sector (e.g., G-10, F-8): ";
        cin >> sector;
        
        // auto-registering sector to tree (cross-sector integration)
        populationSystem->autoRegisterSector(sector);
        
        cout << "Enter rating (0.0-5.0): ";
        cin >> rating;
        
        // Validate rating
        if (!isValidRating(rating)) {
            cout << "\n[ERROR] Invalid rating! Rating must be between 0.0 and 5.0." << endl;
            return;
        }
        cin.ignore();
        
        cout << "Enter subjects (comma-separated): ";
        cin.getline(subjects, 200);
        
        registerSchool(schoolID, name, sector, rating, subjects);
        
        cout << "\n[OK] School registered successfully!" << endl;
    }
    
    // Register student manually (WITH CNIC AND POPULATION INTEGRATION)
    void registerStudentManual() {
        char studentID[50], name[100], cnic[20], schoolID[50], className[50];
        char sector[50], street[50], house[20], gender[10];
        int age;
        
        cout << "\n=== Register New Student ===" << endl;
        
        cout << "Enter student CNIC: ";
        cin >> cnic;
        
        // Validate CNIC format
        if (!isValidCNIC(cnic)) {
            cout << "\n[ERROR] Invalid CNIC format!" << endl;
            cout << "[TIP] CNIC should be: XXXXX-XXXXXXX-X (5 digits-7 digits-1 digit)" << endl;
            return;
        }
        
        // Check if already registered
        if (populationSystem->cnicExists(cnic)) {
            cout << "\n[ERROR] Student with CNIC " << cnic << " already registered!" << endl;
            return;
        }
        
        cin.ignore();
        cout << "Enter name: ";
        cin.getline(name, 100);
        
        cout << "Enter age: ";
        cin >> age;
        
        cout << "Enter school ID: ";
        cin >> schoolID;
        normalizeInput(schoolID);
        
        // making sure Check if school exists
        if (!schoolExists(schoolID)) {
            cout << "\n[ERROR] School " << schoolID << " does not exist!" << endl;
            cout << "[TIP] Register the school first (Option [2])" << endl;
            return;
        }
        
        cout << "Enter class name (e.g., Grade 10): ";
        cin.ignore();
        cin.getline(className, 50);
        
        cout << "Enter gender (Male/Female): ";
        cin >> gender;
        
        cout << "Enter sector (student's home): ";
        cin >> sector;
        
        cout << "Enter street name: ";
        cin.ignore();
        cin.getline(street, 50);
        
        cout << "Enter house number: ";
        cin >> house;
        
        // Auto-register sector
        populationSystem->autoRegisterSector(sector);
        
        // Add to population first (as a Student)
        populationSystem->registerCitizen(cnic, name, age, sector, street, house, "Student", gender);
        
        // Then add to education system
        generateUniqueID("STU", studentID);
        registerStudent(studentID, name, cnic, schoolID, age, className);
        
        cout << "\n[OK] Student registered successfully!" << endl;
        cout << "Student ID: " << studentID << endl;
    }
    
    // Display all students
    void displayAllStudents() {
        cout << "\n========== ALL STUDENTS ==========" << endl;
        cout << "Total Students: " << studentCount << endl;
        
        if (studentCount == 0) {
            cout << "[INFO] No students registered yet." << endl;
            cout << "=====================================" << endl;
            return;
        }
        
        char** keys = new char*[1000];
        int count;
        studentTable->getAllKeys(keys, &count);
        
        cout << "\n";
        for (int i = 0; i < count; i++) {
            Student* student = (Student*)studentTable->search(keys[i]);
            if (student != nullptr) {
                cout << "[" << (i+1) << "] " << student->name << endl;
                cout << "    ID: " << student->studentID << endl;
                cout << "    CNIC: " << student->cnic << endl;
                cout << "    School: " << student->schoolID << endl;
                cout << "    Age: " << student->age << endl;
                cout << "    Class: " << student->className << endl;
                cout << endl;
            }
        }
        
        delete[] keys;
        cout << "=====================================" << endl;
    }
    
    // Search schools by subject offered
    void searchSchoolsBySubject() {
        char subject[100];
        cout << "\n=== Search Schools by Subject ===" << endl;
        cout << "Enter subject name: ";
        cin.ignore();
        cin.getline(subject, 100);
        toUpperCase(subject);
        
        // Get all school keys
        char** schoolKeys = new char*[100];
        int count;
        schoolTable->getAllKeys(schoolKeys, &count);
        
        bool found = false;
        cout << "\n--- Schools Offering " << subject << " ---" << endl;
        
        for (int i = 0; i < count; i++) {
            School* school = (School*)schoolTable->search(schoolKeys[i]);
            if (school != nullptr) {
                // Convert subjects to uppercase for comparison
                char upperSubjects[200];
                stringCopy(upperSubjects, school->subjects);
                toUpperCase(upperSubjects);
                
                // Simple substring search
                bool hasSubject = false;
                int subjLen = stringLength(subject);
                int fullLen = stringLength(upperSubjects);
                for (int j = 0; j <= fullLen - subjLen; j++) {
                    bool match = true;
                    for (int k = 0; k < subjLen; k++) {
                        if (upperSubjects[j + k] != subject[k]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        hasSubject = true;
                        break;
                    }
                }
                
                if (hasSubject) {
                    cout << "  - " << school->name << " (" << school->sector << ")" << endl;
                    cout << "    Rating: " << school->rating << "/5.0" << endl;
                    found = true;
                }
            }
        }
        
        delete[] schoolKeys;
        
        if (!found) {
            cout << "[INFO] No schools found offering this subject." << endl;
        }
    }
    
    // finding nearest school to location
    void findNearestSchool() {
        char userSector[50];
        
        cout << "\n=== Find Nearest School ===" << endl;
        cout << "Enter your sector (e.g., G-10): ";
        cin >> userSector;
        normalizeInput(userSector);
        
        // Get all schools
        char** schoolKeys = new char*[100];
        int count;
        schoolTable->getAllKeys(schoolKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No schools registered yet." << endl;
            delete[] schoolKeys;
            return;
        }
        
        bool found = false;
        cout << "\n--- Schools in Your Sector ---" << endl;
        
        for (int i = 0; i < count; i++) {
            School* school = (School*)schoolTable->search(schoolKeys[i]);
            if (school != nullptr) {
                // Case-insensitive comparison
                char schoolSector[50];
                stringCopy(schoolSector, school->sector);
                normalizeInput(schoolSector);
                
                if (stringCompare(schoolSector, userSector) == 0) {
                    cout << "  - " << school->name << endl;
                    cout << "    Rating: " << school->rating << "/5.0" << endl;
                    found = true;
                }
            }
        }
        
        delete[] schoolKeys;
        
        if (!found) {
            cout << "[INFO] No schools found in your sector." << endl;
        }
    }
    
    // ========== VISUALIZATION HELPER METHODS ==========
    
    // Get all school keys for visualization
    void getSchoolKeys(char** keys, int* count) {
        schoolTable->getAllKeys(keys, count);
    }
    
    // Get school by ID
    School* getSchool(const char* schoolID) {
        return (School*)schoolTable->search(schoolID);
    }
    
    ~EducationSystem() {
        delete schoolTable;
        delete facultyTable;
        delete studentTable;
        delete schoolRanking;
    }
};

#endif
