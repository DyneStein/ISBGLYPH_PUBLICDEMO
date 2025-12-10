#ifndef PUBLICFACILITIES_H
#define PUBLICFACILITIES_H

#include "HashTable.h"
#include "Utils.h"
#include <iostream>
using namespace std;

// Forward declaration
class PopulationSystem;
extern PopulationSystem* populationSystem;

struct Facility {
    char facilityID[50];
    char name[100];
    char type[50];
    char sector[50];
    
    Facility() {
        facilityID[0] = '\0';
        name[0] = '\0';
        type[0] = '\0';
        sector[0] = '\0';
    }
};

// public facility system class
class PublicFacilitySystem {
private:
    HashTable* facilityTable;
    int facilityCount;
    char lastRegisteredSector[50];

public:
    PublicFacilitySystem() {
        facilityTable = new HashTable(100);
        facilityCount = 0;
        lastRegisteredSector[0] = '\0';
    }
    
    // register facility
    void registerFacility(const char* facilityID, const char* name, 
                         const char* type, const char* sector) {
        Facility* facility = new Facility();
        stringCopyN(facility->facilityID, facilityID, 50);
        stringCopyN(facility->name, name, 100);
        stringCopyN(facility->type, type, 50);
        stringCopyN(facility->sector, sector, 50);
        
        facilityTable->insert(facilityID, facility);
        facilityCount++;
        cout << "Registered facility: " << name << " (" << type << ")" << endl;
    }
    
    // Register facility manually
    void registerFacilityManual() {
        char facilityID[50], name[100], type[50], sector[50];
        
        cout << "\n=== Register New Public Facility ===" << endl;
        
        generateUniqueID("FAC", facilityID);
        cout << "Generated Facility ID: " << facilityID << endl;
        
        cout << "Enter facility name: ";
        cin.ignore();
        cin.getline(name, 100);
        
        cout << "Enter type (Mosque/Park/WaterCooler): ";
        cin >> type;
        
        cout << "Enter sector (e.g., G-10, F-8): ";
        cin >> sector;
        stringCopy(lastRegisteredSector, sector);
        
        // auto-registering sector to tree (cross-sector integration)
        populationSystem->autoRegisterSector(sector);
        
        registerFacility(facilityID, name, type, sector);
        cout << "\n[OK] Facility registered successfully!" << endl;
    }
    
    // finding nearest facility by type
    void findNearestFacilityByType() {
        char userSector[50], facilityType[50];
        cout << "\n=== Find Nearest Facility ===" << endl;
        cout << "Enter facility type (Mosque/Park/WaterCooler): ";
        cin.ignore();
        cin.getline(facilityType, 50);
        toUpperCase(facilityType);
        
        cout << "Enter your sector (e.g., G-10): ";
        cin >> userSector;
        normalizeInput(userSector);
        
        char** facKeys = new char*[100];
        int count;
        facilityTable->getAllKeys(facKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No facilities registered yet." << endl;
            delete[] facKeys;
            return;
        }
        
        bool found = false;
        cout << "\n--- " << facilityType << " in Your Sector ---" << endl;
        for (int i = 0; i < count; i++) {
            Facility* fac = (Facility*)facilityTable->search(facKeys[i]);
            if (fac != nullptr) {
                char facType[50], facSector[50];
                stringCopy(facType, fac->type);
                stringCopy(facSector, fac->sector);
                toUpperCase(facType);
                toUpperCase(facSector);
                
                if (stringCompare(facType, facilityType) == 0 && 
                    stringCompare(facSector, userSector) == 0) {
                    cout << "  - " << fac->name << endl;
                    found = true;
                }
            }
        }
        
        delete[] facKeys;
        
        if (!found) {
            cout << "[INFO] No facilities of this type found in your sector." << endl;
        }
    }
    
    // Display by type
    void displayByType() {
        char facilityType[50];
        cout << "\n=== Display Facilities by Type ===" << endl;
        cout << "Enter type (Mosque/Park/WaterCooler): ";
        cin.ignore();
        cin.getline(facilityType, 50);
        toUpperCase(facilityType);
        
        char** facKeys = new char*[100];
        int count;
        facilityTable->getAllKeys(facKeys, &count);
        
        bool found = false;
        cout << "\n--- All " << facilityType << " Facilities ---" << endl;
        for (int i = 0; i < count; i++) {
            Facility* fac = (Facility*)facilityTable->search(facKeys[i]);
            if (fac != nullptr) {
                char facType[50];
                stringCopy(facType, fac->type);
                toUpperCase(facType);
                
                if (stringCompare(facType, facilityType) == 0) {
                    cout << "  - " << fac->name << " (" << fac->sector << ")" << endl;
                    found = true;
                }
            }
        }
        
        delete[] facKeys;
        
        if (!found) {
            cout << "[INFO] No facilities of this type found." << endl;
        }
    }
    
    // Get count
    int getFacilityCount() { return facilityCount; }
    const char* getLastRegisteredSector() const { return lastRegisteredSector; }
    
    // Display all summary
    void displayAllSummary() {
        cout << "\n========== PUBLIC FACILITIES SUMMARY ==========" << endl;
        cout << "Total Facilities: " << facilityCount << endl;
        cout << "=============================================" << endl;
    }
    
    // Display all facilities
    void displayAllFacilities() {
        cout << "\n========== ALL PUBLIC FACILITIES ==========" << endl;
        
        char** facKeys = new char*[100];
        int count;
        facilityTable->getAllKeys(facKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No facilities registered yet." << endl;
            delete[] facKeys;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            Facility* fac = (Facility*)facilityTable->search(facKeys[i]);
            if (fac != nullptr) {
                cout << "\n[" << (i+1) << "] " << fac->name << endl;
                cout << "    ID: " << fac->facilityID << endl;
                cout << "    Type: " << fac->type << endl;
                cout << "    Sector: " << fac->sector << endl;
            }
        }
        
        delete[] facKeys;
        cout << "\n==========================================" << endl;
    }
    
    void displayFacility(const char* facilityID) {
        Facility* fac = (Facility*)facilityTable->search(facilityID);
        if (fac == nullptr) {
            cout << "Facility not found!" << endl;
            return;
        }
        
        cout << "\n--- Facility Details ---" << endl;
        cout << "Facility ID: " << fac->facilityID << endl;
        cout << "Name: " << fac->name << endl;
        cout << "Type: " << fac->type << endl;
        cout << "Sector: " << fac->sector << endl;
    }
    
    // Park visualization helpers
    void getParkKeys(char** keys, int* count) {
        char** allKeys = new char*[100];
        int totalCount = 0;
        facilityTable->getAllKeys(allKeys, &totalCount);
        
        *count = 0;
        for (int i = 0; i < totalCount && *count < 100; i++) {
            Facility* fac = (Facility*)facilityTable->search(allKeys[i]);
            if (fac && stringCompareIgnoreCase(fac->type, "Park") == 0) {
                keys[*count] = allKeys[i];
                (*count)++;
            }
        }
    }
    
    Facility* getPark(const char* parkID) {
        Facility* fac = (Facility*)facilityTable->search(parkID);
        if (fac && stringCompareIgnoreCase(fac->type, "Park") == 0) {
            return fac;
        }
        return nullptr;
    }
    
    // Mosque visualization helpers
    void getMosqueKeys(char** keys, int* count) {
        char** allKeys = new char*[100];
        int totalCount = 0;
        facilityTable->getAllKeys(allKeys, &totalCount);
        
        *count = 0;
        for (int i = 0; i < totalCount && *count < 100; i++) {
            Facility* fac = (Facility*)facilityTable->search(allKeys[i]);
            if (fac && stringCompareIgnoreCase(fac->type, "Mosque") == 0) {
                keys[*count] = allKeys[i];
                (*count)++;
            }
        }
    }
    
    Facility* getMosque(const char* mosqueID) {
        Facility* fac = (Facility*)facilityTable->search(mosqueID);
        if (fac && stringCompareIgnoreCase(fac->type, "Mosque") == 0) {
            return fac;
        }
        return nullptr;
    }
    
    ~PublicFacilitySystem() {
        delete facilityTable;
    }
};

#endif
