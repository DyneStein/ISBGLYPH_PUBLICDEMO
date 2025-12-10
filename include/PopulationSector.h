#ifndef POPULATIONSECTOR_H
#define POPULATIONSECTOR_H

#include "Tree.h"
#include "HashTable.h"
#include "Utils.h"
#include <iostream>
using namespace std;

struct Citizen {
    char cnic[20];
    char name[100];
    int age;
    char sector[50];
    char street[50];
    char houseNo[20];
    char occupation[100];
    char gender[10];
    
    Citizen() {
        cnic[0] = '\0';
        name[0] = '\0';
        age = 0;
        sector[0] = '\0';
        street[0] = '\0';
        houseNo[0] = '\0';
        occupation[0] = '\0';
        gender[0] = '\0';
    }
};

class PopulationSystem {
private:
    Tree* sectorHierarchy;
    HashTable* citizenTable;
    int totalPopulation;
    
    // keeping count of stuff
    int engineerCount;
    int doctorCount;
    int teacherCount;
    int studentCount;
    int businessCount;
    int otherCount;
    
    int age0to18;
    int age19to35;
    int age36to60;
    int ageAbove60;
    
    int maleCount;
    int femaleCount;

public:
    PopulationSystem() {
        sectorHierarchy = new Tree();
        citizenTable = new HashTable(1000);
        totalPopulation = 0;
        
        engineerCount = 0;
        doctorCount = 0;
        teacherCount = 0;
        studentCount = 0;
        businessCount = 0;
        otherCount = 0;
        
        age0to18 = 0;
        age19to35 = 0;
        age36to60 = 0;
        ageAbove60 = 0;
        
        maleCount = 0;
        femaleCount = 0;
        
        // initialize islamabad root
        sectorHierarchy->setRoot(0, "Islamabad");
    }
    
    // PUBLIC METHOD: Auto-register sector (for other modules to use)
    void autoRegisterSector(const char* sectorName) {
        // Check if already exists first
        TreeNode* existing = sectorHierarchy->find(sectorName);
        if (existing != nullptr) {
            return;  // Already registered, skip
        }
        
        // Parse sector notation if it contains '/'
        char sector[50], subsector[50];
        parseSectorNotation(sectorName, sector, subsector);
        
        // Add main sector
        bool added = sectorHierarchy->addNode("Islamabad", 0, sector);
        // if (added) {
        //     cout << "Auto-registered sector: " << sector << endl;
        // }
        
        // If subsector provided, add it
        if (subsector[0] != '\0') {
            char fullSubsector[100];
            stringCopy(fullSubsector, sector);
            stringConcat(fullSubsector, "/");
            stringConcat(fullSubsector, subsector);
            
            bool subAdded = sectorHierarchy->addNode(sector, 0, fullSubsector);
            // if (subAdded) {
            //     cout << "Auto-registered subsector: " << fullSubsector << endl;
            // }
        }
    }
    
    // CNIC Validation Helpers (for cross-sector integration)
    bool cnicExists(const char* cnic) {
        return citizenTable->contains(cnic);
    }
    
    Citizen* getCitizenByCNIC(const char* cnic) {
        return (Citizen*)citizenTable->search(cnic);
    }
    
    // Update citizen address
    void updateCitizenAddress(const char* cnic, const char* sector, 
                             const char* street, const char* house) {
        Citizen* citizen = getCitizenByCNIC(cnic);
        if (citizen != nullptr) {
            stringCopyN(citizen->sector, sector, 50);
            stringCopyN(citizen->street, street, 50);
            stringCopyN(citizen->houseNo, house, 20);
            
            // Auto-register new sector
            autoRegisterSector(sector);
        }
    }
    
    void addSector(const char* sectorName) {
        bool added = sectorHierarchy->addNode("Islamabad", 0, sectorName);
        if (added) {
            // cout << "Added sector: " << sectorName << endl;
        }
    }
    
    void addStreet(const char* sectorName, const char* streetName) {
        sectorHierarchy->addNode(sectorName, 0, streetName);
        cout << "Added street: " << streetName << " to " << sectorName << endl;
    }
    
    // Add citizen to population (keeping it simple version for manual family registration)
    void addCitizen(const char* cnic, const char* name, int age, const char* occupation) {
        Citizen* citizen = new Citizen();
        stringCopyN(citizen->cnic, cnic, 20);
        stringCopyN(citizen->name, name, 100);
        citizen->age = age;
        stringCopyN(citizen->occupation, occupation, 100); // Use 100 for occupation
        
        // Default other fields for this keeping it simple addCitizen
        stringCopyN(citizen->sector, "N/A", 50);
        stringCopyN(citizen->street, "N/A", 50);
        stringCopyN(citizen->houseNo, "N/A", 20);
        stringCopyN(citizen->gender, "N/A", 10);
        
        citizenTable->insert(cnic, citizen); // Use citizenTable
        totalPopulation++;
        
        // Update occupation counters (keeping it simple)
        if (stringCompare(occupation, "Engineer") == 0) engineerCount++;
        else if (stringCompare(occupation, "Doctor") == 0) doctorCount++;
        else if (stringCompare(occupation, "Teacher") == 0) teacherCount++;
        else if (stringCompare(occupation, "Student") == 0) studentCount++;
        else if (stringCompare(occupation, "Business") == 0) businessCount++;
        else otherCount++;
        
        // Update age groups (keeping it simple)
        if (age <= 18) age0to18++;
        else if (age <= 35) age19to35++;
        else if (age <= 60) age36to60++;
        else ageAbove60++;
    }
    
    // ========== DYNAMIC SECTOR MANAGEMENT ==========
    
    // Register sector dynamically (parses notation like "G-10/4")
    void registerSectorDynamic() {
        char sectorInput[100], sector[50], subsector[50];
        
        cout << "\n=== Register New Sector ===" << endl;
        cout << "Enter sector notation (e.g., G-10 or G-10/4): ";
        cin >> sectorInput;
        
        parseSectorNotation(sectorInput, sector, subsector);
        
        // Use autoRegisterSector which checks for duplicates
        autoRegisterSector(sectorInput);
        
        cout << "\n[OK] Sector registration complete!" << endl;
    }
    
    // View sector hierarchy tree
    void viewSectorHierarchy() {
        cout << "\n=== Sector Hierarchy ==="<< endl;
        cout << "Root: Islamabad" << endl;
        // Tree::display() method can be called if implemented
        sectorHierarchy->display(); // Call the existing display method
        cout << "\nSector tree structure is maintained dynamically." << endl;
    }
    
    // Register family manually
    void registerFamilyManual() {
        char cnic[20], name[100], occupation[100];
        char sector[50], street[50], house[20], gender[10];
        int age;
        
        cout << "\n=== Register Family Member ===" << endl;
        cout << "Enter CNIC (e.g., 12345-1234567-1): ";
        cin >> cnic;
        
        // making sure Check if already registered
        if (cnicExists(cnic)) {
            cout << "\n[ERROR] Person with CNIC " << cnic << " already registered!" << endl;
            cout << "[TIP] Use 'Lookup by CNIC' to view existing person" << endl;
            return;
        }
        
        cin.ignore();
        
        cout << "Enter name: ";
        cin.getline(name, 100);
        
        cout << "Enter age: ";
        cin >> age;
        cin.ignore();
        
        cout << "Enter occupation: ";
        cin.getline(occupation, 100);
        
        cout << "Enter gender (Male/Female): ";
        cin >> gender;
        
        cout << "Enter sector (e.g., G-10): ";
        cin >> sector;
        
        cout << "Enter street name: ";
        cin.ignore();
        cin.getline(street, 50);
        
        cout << "Enter house number: ";
        cin >> house;
        
        // Auto-register sector
        autoRegisterSector(sector);
        
        // Register citizen with full details
        registerCitizen(cnic, name, age, sector, street, house, occupation, gender);
        cout << "\n[OK] Family member registered successfully!" << endl;
    }
    
    // Lookup citizen by CNIC
    void lookupByCNIC() {
        char cnic[20];
        cout << "\n=== Lookup Citizen by CNIC ===" << endl;
        cout << "Enter CNIC: ";
        cin >> cnic;
        
        Citizen* citizen = (Citizen*)citizenTable->search(cnic); // Use citizenTable
        if (citizen == nullptr) {
            cout << "[INFO] Citizen not found." << endl;
            return;
        }
        
        cout << "\n--- Citizen Details ---" << endl;
        cout << "CNIC: " << citizen->cnic << endl;
        cout << "Name: " << citizen->name << endl;
        cout << "Age: " << citizen->age << endl;
        cout << "Occupation: " << citizen->occupation << endl;
        // Display other fields if available from addCitizen (N/A if not set)
        cout << "Gender: " << citizen->gender << endl;
        cout << "Address: House " << citizen->houseNo << ", Street " 
             << citizen->street << ", " << citizen->sector << endl;
    }
    
    // Update citizen address
    void updateCitizenAddressManual() {
        char cnic[20], sector[50], street[50], house[20];
        
        cout << "\n=== Update Citizen Address ===" << endl;
        cout << "Enter CNIC: ";
        cin >> cnic;
        
        if (!cnicExists(cnic)) {
            cout << "[ERROR] Citizen with CNIC " << cnic << " not found!" << endl;
            return;
        }
        
        cout << "Enter new sector (e.g., G-10): ";
        cin >> sector;
        
        cout << "Enter new street name: ";
        cin.ignore();
        cin.getline(street, 50);
        
        cout << "Enter new house number: ";
        cin >> house;
        
        updateCitizenAddress(cnic, sector, street, house);
        cout << "\n[OK] Address updated successfully!" << endl;
    }
    
    // Generate reports
    void generateAgeDistribution() {
        cout << "\n=== Age Distribution Report ===" << endl;
        cout << "Total Population: " << totalPopulation << endl;
        cout << "Age 0-18: " << age0to18 << " (" 
             << (totalPopulation > 0 ? (age0to18 * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
        cout << "Age 19-35: " << age19to35 << " (" 
             << (totalPopulation > 0 ? (age19to35 * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
        cout << "Age 36-60: " << age36to60 << " (" 
             << (totalPopulation > 0 ? (age36to60 * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
        cout << "Age 60+: " << ageAbove60 << " (" 
             << (totalPopulation > 0 ? (ageAbove60 * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
    }
    
    void generateOccupationReport() {
        cout << "\n=== Occupation Distribution Report ===" << endl;
        cout << "Total Population: " << totalPopulation << endl;
        cout << "Engineers: " << engineerCount << endl;
        cout << "Doctors: " << doctorCount << endl;
        cout << "Teachers: " << teacherCount << endl;
        cout << "Students: " << studentCount << endl;
        cout << "Business: " << businessCount << endl;
        cout << "Others: " << otherCount << endl;
    }
    
    // Original registerCitizen method (kept for full functionality)
    void registerCitizen(const char* cnic, const char* name, int age, 
                        const char* sector, const char* street, const char* houseNo,
                        const char* occupation, const char* gender) {
        Citizen* citizen = new Citizen();
        stringCopyN(citizen->cnic, cnic, 20);
        stringCopyN(citizen->name, name, 100);
        citizen->age = age;
        stringCopyN(citizen->sector, sector, 50);
        stringCopyN(citizen->street, street, 50);
        stringCopyN(citizen->houseNo, houseNo, 20);
        stringCopyN(citizen->occupation, occupation, 100);
        stringCopyN(citizen->gender, gender, 10);
        
        citizenTable->insert(cnic, citizen);
        totalPopulation++;
        
        // update occupation counters
        if (stringCompare(occupation, "Engineer") == 0) engineerCount++;
        else if (stringCompare(occupation, "Doctor") == 0) doctorCount++;
        else if (stringCompare(occupation, "Teacher") == 0) teacherCount++;
        else if (stringCompare(occupation, "Student") == 0) studentCount++;
        else if (stringCompare(occupation, "Business") == 0) businessCount++;
        else otherCount++;
        
        // update age groups
        if (age <= 18) age0to18++;
        else if (age <= 35) age19to35++;
        else if (age <= 60) age36to60++;
        else ageAbove60++;
        
        // update gender
        if (stringCompare(gender, "Male") == 0) maleCount++;
        else if (stringCompare(gender, "Female") == 0) femaleCount++;
        
        // Registered citizen: (quiet mode)
        // cout << "Registered citizen: " << name << " (CNIC: " << cnic << ")" << endl;
    }
    
    void lookupCitizen(const char* cnic) {
        // Validate CNIC format
        if (!isValidCNIC(cnic)) {
            cout << "\n[ERROR] Invalid CNIC format!" << endl;
            cout << "[TIP] CNIC format should be: XXXXX-XXXXXXX-X (e.g., 12345-6789012-3)" << endl;
            cout << "      - 5 digits, dash, 7 digits, dash, 1 digit" << endl;
            cout << "      - No alphabets allowed, only numbers and dashes" << endl;
            return;
        }
        
        Citizen* citizen = (Citizen*)citizenTable->search(cnic);
        if (citizen == nullptr) {
            cout << "Citizen not found!" << endl;
            return;
        }
        
        cout << "\n--- Citizen Details ---" << endl;
        cout << "CNIC: " << citizen->cnic << endl;
        cout << "Name: " << citizen->name << endl;
        cout << "Age: " << citizen->age << endl;
        cout << "Gender: " << citizen->gender << endl;
        cout << "Address: House " << citizen->houseNo << ", Street " 
             << citizen->street << ", " << citizen->sector << endl;
        cout << "Occupation: " << citizen->occupation << endl;
    }
    
    void displaySectorHierarchy() {
        cout << "\n--- Islamabad Sector Hierarchy ---" << endl;
        sectorHierarchy->display();
    }
    
    void generateAgeReport() {
        cout << "\n--- Age Distribution Report ---" << endl;
        cout << "Total Population: " << totalPopulation << endl;
        cout << "Age 0-18: " << age0to18 << " (" 
             << (totalPopulation > 0 ? (age0to18 * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
        cout << "Age 19-35: " << age19to35 << " (" 
             << (totalPopulation > 0 ? (age19to35 * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
        cout << "Age 36-60: " << age36to60 << " (" 
             << (totalPopulation > 0 ? (age36to60 * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
        cout << "Age 60+: " << ageAbove60 << " (" 
             << (totalPopulation > 0 ? (ageAbove60 * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
    }
    
    // Display all citizens in organized format
    void displayAllCitizens() {
        cout << "\n========== ALL REGISTERED CITIZENS ==========" << endl;
        cout << "Total Population: " << totalPopulation << endl;
        
        if (totalPopulation == 0) {
            cout << "[INFO] No citizens registered yet." << endl;
            cout << "=============================================" << endl;
            return;
        }
        
        // Get all citizen keys from hash table
        char** citizenKeys = new char*[1000];
        int count;
        citizenTable->getAllKeys(citizenKeys, &count);
        
        cout << "\n--- Citizen List ---" << endl;
        for (int i = 0; i < count; i++) {
            Citizen* citizen = (Citizen*)citizenTable->search(citizenKeys[i]);
            if (citizen != nullptr) {
                cout << "\n[" << (i+1) << "] CNIC: " << citizen->cnic << endl;
                cout << "    Name: " << citizen->name << endl;
                cout << "    Age: " << citizen->age << endl;
                cout << "    Gender: " << citizen->gender << endl;
                cout << "    Occupation: " << citizen->occupation << endl;
                cout << "    Address: House " << citizen->houseNo << ", " 
                     << citizen->street << ", " << citizen->sector << endl;
            }
        }
        
        delete[] citizenKeys;
        cout << "\n=============================================" << endl;
    }
    
    // Get total population
    int getTotalPopulation() { return totalPopulation; }
    
    void generateGenderReport() {
        cout << "\n--- Gender Ratio Report ---" << endl;
        cout << "Total Population: " << totalPopulation << endl;
        cout << "Male: " << maleCount << " (" 
             << (totalPopulation > 0 ? (maleCount * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
        cout << "Female: " << femaleCount << " (" 
             << (totalPopulation > 0 ? (femaleCount * 100.0 / totalPopulation) : 0) 
             << "%)" << endl;
    }
    
    // bonus feature FEATURE: Population Heatmap
    void generatePopulationHeatmap() {
        cout << "\n========================================" << endl;
        cout << "   POPULATION DENSITY HEATMAP" << endl;
        cout << "========================================" << endl;
        cout << "Legend: # Very High  = High  : Medium  . Low  (space) Very Low" << endl;
        cout << "----------------------------------------" << endl;
        
        // Simple sector distribution - show total population
        cout << "\nTotal Population: " << totalPopulation << " citizens" << endl;
        cout << "Distributed across 5 main sectors" << endl;
        cout << "\nSector-wise Distribution:\n" << endl;
        
        // Simplified - show average per sector
        int avgPerSector = totalPopulation / 5;  // 5 main sectors
        
        cout << "G-10: ";
        for (int i = 0; i < avgPerSector && i < 50; i++) cout << ":";
        cout << " (" << avgPerSector << " citizens)" << endl;
        
        cout << "F-8: ";
        for (int i = 0; i < avgPerSector && i < 50; i++) cout << ":";
        cout << " (" << avgPerSector << " citizens)" << endl;
        
        cout << "G-9: ";
        for (int i = 0; i < avgPerSector && i < 50; i++) cout << ":";
        cout << " (" << avgPerSector << " citizens)" << endl;
        
        cout << "F-6: ";
        for (int i = 0; i < avgPerSector && i < 50; i++) cout << ":";
        cout << " (" << avgPerSector << " citizens)" << endl;
        
        cout << "Blue Area: ";
        for (int i = 0; i < avgPerSector && i < 50; i++) cout << ":";
        cout << " (" << avgPerSector << " citizens)" << endl;
        
        cout << "\n2D Grid Visualization (10x10 sectors):" << endl;
        cout << "+----+----+----+----+----+----+----+----+----+----+" << endl;
        
        // calculate density for cells
        int cellPop = totalPopulation / 100;  // divide among 100 cells
        
        // create 10x10 grid with simple ASCII based on population density
        for (int row = 0; row < 10; row++) {
            cout << "|";
            for (int col = 0; col < 10; col++) {
                // use simple ASCII characters
                char symbol;
                if (cellPop > 20) symbol = '#';       // Very High
                else if (cellPop > 10) symbol = '=';  // High
                else if (cellPop > 5) symbol = ':';   // Medium
                else if (cellPop > 2) symbol = '.';   // Low
                else symbol = ' ';                    // Very Low (space)
                
                cout << " " << symbol << symbol << " |";
            }
            cout << endl;
            cout << "+----+----+----+----+----+----+----+----+----+----+" << endl;
        }
        
        cout << "\nAverage Density: " << cellPop << " per grid cell" << endl;
    }
    
    // bonus feature FEATURE: Family Tree Visualization
    void displayFamilyTree(const char* searchCNIC) {
        // Validate CNIC format
        if (!isValidCNIC(searchCNIC)) {
            cout << "\n[ERROR] Invalid CNIC format!" << endl;
            cout << "[TIP] CNIC format should be: XXXXX-XXXXXXX-X (e.g., 12345-6789012-3)" << endl;
            return;
        }
        
        Citizen* searchedPerson = (Citizen*)citizenTable->search(searchCNIC);
        if (searchedPerson == nullptr) {
            cout << "[ERROR] Citizen with CNIC " << searchCNIC << " not found!" << endl;
            cout << "[TIP] Use 'Lookup Citizen by CNIC' to verify CNIC exists" << endl;
            return;
        }
        
        cout << "\n========================================" << endl;
        cout << "   FAMILY TREE VISUALIZATION" << endl;
        cout << "========================================" << endl;
        cout << "Searched Person: " << searchedPerson->name << endl;
        cout << "Address: " << searchedPerson->sector << ", " 
             << searchedPerson->street << ", House " << searchedPerson->houseNo << endl;
        cout << "----------------------------------------\n" << endl;
        
        // Find ALL family members at same address
        char** allKeys = new char*[1000];
        int count;
        citizenTable->getAllKeys(allKeys, &count);
        
        // Store all family members
        Citizen* familyMembers[50];
        int familyCount = 0;
        
        for (int i = 0; i < count && familyCount < 50; i++) {
            Citizen* citizen = (Citizen*)citizenTable->search(allKeys[i]);
            if (citizen != nullptr) {
                // Check if same address
                if (stringCompare(citizen->sector, searchedPerson->sector) == 0 &&
                    stringCompare(citizen->street, searchedPerson->street) == 0 &&
                    stringCompare(citizen->houseNo, searchedPerson->houseNo) == 0) {
                    familyMembers[familyCount] = citizen;
                    familyCount++;
                }
            }
        }
        
        cout << "Family Hierarchy (Tree Structure):" << endl;
        cout << "Total Family Members: " << familyCount << endl << endl;
        
        // Display all family members with complete details
        for (int i = 0; i < familyCount; i++) {
            Citizen* member = familyMembers[i];
            
            if (i == 0) {
                // First member shown as root
                cout << member->name << " (" << member->age << " years)" << endl;
            } else {
                cout << "  |" << endl;
                cout << "  +-- " << member->name << " (" << member->age << " years)" << endl;
            }
            
            cout << (i == 0 ? "  |" : "      |") << endl;
            cout << (i == 0 ? "  +-- CNIC: " : "      +-- CNIC: ") << member->cnic << endl;
            cout << (i == 0 ? "  +-- Gender: " : "      +-- Gender: ") << member->gender << endl;
            cout << (i == 0 ? "  +-- Occupation: " : "      +-- Occupation: ") << member->occupation << endl;
            cout << (i == 0 ? "  +-- Address: House " : "      +-- Address: House ") 
                 << member->houseNo << ", " << member->street << endl;
            
            if (i < familyCount - 1) {
                cout << endl;
            }
        }
        
        delete[] allKeys;
        
        cout << "\n========================================" << endl;
        cout << "Total Family Size: " << familyCount << " member(s)" << endl;
        cout << "========================================" << endl;
        cout << "[DEMO] Tree structure used for hierarchical display!" << endl;
        cout << "[NOTE] Search works with ANY family member's CNIC!" << endl;
        cout << "========================================" << endl;
    }
    
    // ========== VISUALIZATION HELPER METHODS ==========
    
    // Get all registered sector names for visualization
    void getSectorNames(char** sectorNames, int* count) {
        *count = 0;
        TreeNode* root = sectorHierarchy->getRoot();
        if (root == nullptr) return;
        
        // Get all child sectors of Islamabad root
        for (int i = 0; i < root->childCount && *count < 50; i++) {
            TreeNode* sector = root->children[i];
            if (sector != nullptr) {
                // Allocate and copy sector name
                int len = stringLength(sector->name);
                sectorNames[*count] = new char[len + 1];
                stringCopy(sectorNames[*count], sector->name);
                (*count)++;
            }
        }
    }
    
    ~PopulationSystem() {
        delete sectorHierarchy;
        delete citizenTable;
    }
};

#endif
