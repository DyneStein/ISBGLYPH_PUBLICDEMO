// ISBGlyph - Main Entry Point
// Muhammad Dyen Asif | Ali Ather | Muhammad Saad

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include <iostream>
#include <fstream>
using namespace std;

#include "include/Graph.h"
#include "include/Utils.h"
#include "include/PopulationSector.h"

// declaring global stuff, pointer so other sectors can use it
PopulationSystem* populationSystem = nullptr;

// now including other sectors that reference populationSystem
#include "include/TransportSector.h"
#include "include/EducationSector.h"
#include "include/MedicalSector.h"
#include "include/AirportSector.h"
#include "include/RailwaySector.h"
#include "include/CommercialSector.h"
#include "include/TransportMenus.h"
#include "include/PublicFacilities.h"

// visualization
#include "include/Visualization.h"

// global stuff for all systems
EducationSystem* educationSystem = nullptr;
MedicalSystem* medicalSystem = nullptr;
TransportSystem* transportSystem = nullptr;
AirportSystem* airportSystem = nullptr;
RailwaySystem* railwaySystem = nullptr;
CommercialSystem* commercialSystem = nullptr;
PublicFacilitySystem* publicFacilities = nullptr;

// Visualization pointer (lazy initialized)
Visualization* viz = nullptr;
bool vizReady = false;

// Helper function to initialize visualization
bool initVisualization() {
    if (viz == nullptr) {
        viz = new Visualization();
        vizReady = viz->init("assets");
        if (vizReady) {
            // CRITICAL: Collect sectors from ALL systems FIRST
            viz->collectAllSectors(populationSystem, transportSystem, 
                                  medicalSystem, educationSystem);
            
            // Set ALL system pointers
            viz->setTransportSystem(transportSystem);
            viz->setMedicalSystem(medicalSystem);
            viz->setEducationSystem(educationSystem);
            viz->setCommercialSystem(commercialSystem);
            viz->setPublicFacilities(publicFacilities);
            viz->setAirportSystem(airportSystem);
            viz->setRailwaySystem(railwaySystem);
            
            cout << "[OK] Visualization initialized!" << endl;
        } else {
            cout << "[ERROR] Visualization failed to initialize!" << endl;
        }
    }
    return vizReady;
}

// Helper to collect all unique sectors from all data sources
void collectAllSectorsForDistribution(char** sectors, int* count) {
    *count = 0;
    
    // Helper to add sector if unique (case-insensitive, whitespace-trimmed)
    auto addUnique = [&](const char* sector) {
        if (sector == nullptr || sector[0] == '\0') return;
        
        // Trim whitespace
        char trimmed[100];
        int start = 0;
        while (sector[start] == ' ') start++;
        
        int len = strlen(sector + start);
        while (len > 0 && sector[start + len - 1] == ' ') len--;
        
        for (int i = 0; i < len; i++) {
            trimmed[i] = sector[start + i];
        }
        trimmed[len] = '\0';
        
        if (trimmed[0] == '\0') return;
        
        // Check if unique (case-insensitive)
        for (int i = 0; i < *count; i++) {
            if (stringCompare(sectors[i], trimmed) == 0) return;
        }
        
        if (*count < 50) {
            int trimmedLen = strlen(trimmed);
            sectors[*count] = new char[trimmedLen + 1];
            strcpy(sectors[*count], trimmed);
            (*count)++;
        }
    };
    
    // Collect from all hospitals
    if (medicalSystem) {
        char** keys = new char*[20];
        int hospCount = 0;
        medicalSystem->getHospitalKeys(keys, &hospCount);
        for (int i = 0; i < hospCount; i++) {
            Hospital* h = medicalSystem->getHospital(keys[i]);
            if (h) addUnique(h->sector);
        }
        delete[] keys;
    }
    
    // Collect from all schools
    if (educationSystem) {
        char** keys = new char*[20];
        int schoolCount = 0;
        educationSystem->getSchoolKeys(keys, &schoolCount);
        for (int i = 0; i < schoolCount; i++) {
            School* s = educationSystem->getSchool(keys[i]);
            if (s) addUnique(s->sector);
        }
        delete[] keys;
    }
    
    // Collect from PopulationSystem
    if (populationSystem) {
        char** keys = new char*[50];
        int popCount = 0;
        populationSystem->getSectorNames(keys, &popCount);
        for (int i = 0; i < popCount; i++) {
            addUnique(keys[i]);
            delete[] keys[i];
        }
        delete[] keys;
    }
}

// load buses from csv
void loadBuses() {
    ifstream file("SmartCity_dataset/buses.csv");
    if (!file.is_open()) {
        cout << "Could not open buses.csv" << endl;
        return;
    }
    
    char line[500];
    file.getline(line, 500);
    
    while (file.getline(line, 500)) {
        char tokens[10][200];
        int count = parseCSVLine(line, tokens, 10);
        
        if (count >= 4) {
            transportSystem->registerBus(tokens[0], tokens[1], tokens[2]);
            
            char* routeStr = tokens[3];
            char stopID[50];
            int idx = 0;
            int stopIdx = 0;
            
            while (routeStr[idx] != '\0') {
                if (routeStr[idx] == '>') {
                    stopID[stopIdx] = '\0';
                    trim(stopID);
                    transportSystem->addStopToRoute(tokens[0], stopID);
                    stopIdx = 0;
                    idx++;
                    while (routeStr[idx] == ' ') idx++;
                } else {
                    stopID[stopIdx++] = routeStr[idx++];
                }
            }
            stopID[stopIdx] = '\0';
            trim(stopID);
            transportSystem->addStopToRoute(tokens[0], stopID);
        }
    }
}

// load stops from csv
void loadStops() {
    ifstream file("SmartCity_dataset/stops.csv");
    if (!file.is_open()) {
        cout << "Could not open stops.csv" << endl;
        return;
    }
    
    char line[500];
    file.getline(line, 500);
    
    // Get sectors from ALL data sources (hospitals, schools, population, etc.)
    char** sectors = new char*[50];
    int sectorCount = 0;
    collectAllSectorsForDistribution(sectors, &sectorCount);
    
    if (sectorCount == 0) {
        cout << "[WARNING] No sectors found in any system. Using default." << endl;
        sectorCount = 1;
        sectors[0] = new char[10];
        strcpy(sectors[0], "G-10");
    }
    
    printf("[OK] Using %d unique sectors for bus stop distribution\n", sectorCount);
    
    int stopIndex = 0;
    
    while (file.getline(line, 500)) {
        char tokens[10][200];
        int count = parseCSVLine(line, tokens, 10);
        
        if (count >= 3) {
            char* coordStr = tokens[2];
            char latStr[50], lonStr[50];
            int i = 0, j = 0;
            
            while (coordStr[i] != ',' && coordStr[i] != '\0') {
                latStr[j++] = coordStr[i++];
            }
            latStr[j] = '\0';
            
            if (coordStr[i] == ',') i++;
            while (coordStr[i] == ' ') i++;
            
            j = 0;
            while (coordStr[i] != '\0') {
                lonStr[j++] = coordStr[i++];
            }
            lonStr[j] = '\0';
            
            double lat = stringToDouble(latStr);
            double lon = stringToDouble(lonStr);
            
            // Round-robin across ALL collected sectors
            const char* assignedSector = sectors[stopIndex % sectorCount];
            transportSystem->addBusStop(tokens[0], tokens[1], assignedSector, lat, lon);
            stopIndex++;
        }
    }
    
    // Clean up
    for (int i = 0; i < sectorCount; i++) {
        delete[] sectors[i];
    }
    delete[] sectors;
    
    file.close();
    
    transportSystem->connectStops("Stop1", "Stop2", 2.5);
    transportSystem->connectStops("Stop2", "Stop3", 3.0);
    transportSystem->connectStops("Stop3", "Stop4", 1.8);
    transportSystem->connectStops("Stop1", "Stop5", 2.2);
    transportSystem->connectStops("Stop5", "Stop8", 4.5);
    transportSystem->connectStops("Stop6", "Stop2", 3.5);
    transportSystem->connectStops("Stop2", "Stop7", 2.0);
    transportSystem->connectStops("Stop7", "Stop9", 5.0);
    transportSystem->connectStops("Stop4", "Stop10", 2.8);
    transportSystem->connectStops("Stop10", "Stop11", 3.2);
    transportSystem->connectStops("Stop8", "Stop6", 2.5);
    transportSystem->connectStops("Stop6", "Stop3", 1.5);
}

// load schools from csv
void loadSchools() {
    ifstream file("SmartCity_dataset/schools.csv");
    if (!file.is_open()) {
        cout << "Could not open schools.csv" << endl;
        return;
    }
    
    char line[500];
    file.getline(line, 500);
    
    while (file.getline(line, 500)) {
        char tokens[10][200];
        int count = parseCSVLine(line, tokens, 10);
        
        if (count >= 5) {
            double rating = stringToDouble(tokens[3]);
            educationSystem->registerSchool(tokens[0], tokens[1], tokens[2], rating, tokens[4]);
            educationSystem->addDepartment(tokens[0], "Primary");
            educationSystem->addDepartment(tokens[0], "Secondary");
        }
    }
}

// load hospitals from csv
void loadHospitals() {
    ifstream file("SmartCity_dataset/hospitals.csv");
    if (!file.is_open()) {
        cout << "Could not open hospitals.csv" << endl;
        return;
    }
    
    char line[500];
    file.getline(line, 500);
    
    while (file.getline(line, 500)) {
        char tokens[10][200];
        int count = parseCSVLine(line, tokens, 10);
        
        if (count >= 5) {
            int emergencyBeds = stringToInt(tokens[3]);
            populationSystem->autoRegisterSector(tokens[2]);
            medicalSystem->registerHospital(tokens[0], tokens[1], tokens[2], emergencyBeds, tokens[4]);
        }
    }
}

// load pharmacies from csv
void loadPharmacies() {
    ifstream file("SmartCity_dataset/pharmacies.csv");
    if (!file.is_open()) {
        cout << "Could not open pharmacies.csv" << endl;
        return;
    }
    
    char line[500];
    file.getline(line, 500);
    
    while (file.getline(line, 500)) {
        char tokens[10][200];
        int count = parseCSVLine(line, tokens, 10);
        
        if (count >= 6) {
            populationSystem->autoRegisterSector(tokens[2]);
            medicalSystem->registerPharmacy(tokens[0], tokens[1], tokens[2]);
            double price = stringToDouble(tokens[5]);
            medicalSystem->addMedicine(tokens[3], tokens[4], price, tokens[0]);
        }
    }
}

// load population from csv
void loadPopulation() {
    ifstream file("SmartCity_dataset/population.csv");
    if (!file.is_open()) {
        cout << "Could not open population.csv" << endl;
        return;
    }
    
    char line[500];
    file.getline(line, 500);
    
    while (file.getline(line, 500)) {
        char tokens[10][200];
        int count = parseCSVLine(line, tokens, 10);
        
        if (count >= 7) {
            int age = stringToInt(tokens[2]);
            populationSystem->autoRegisterSector(tokens[3]);
            char gender[10] = "Male";
            populationSystem->registerCitizen(tokens[0], tokens[1], age,
                tokens[3], tokens[4], tokens[5], tokens[6], gender);
        }
    }
}

void displayMainMenu() {
    cout << "\n+========================================================+" << endl;
    cout << "|                                                        |" << endl;
    cout << "|                 ** ISBGlyph **                          |" << endl;
    cout << "|             >>> Islamabad Edition <<<                  |" << endl;
    cout << "|                                                        |" << endl;
    cout << "+========================================================+" << endl;
    cout << "|                   MAIN MODULES                         |" << endl;
    cout << "+========================================================+" << endl;
    cout << "|  [1] Transport & Routes     (Buses, Airplanes, Trains) |" << endl;
    cout << "|  [2] Education System         (Schools & Students)     |" << endl;
    cout << "|  [3] Medical Services         (Hospitals & Meds)       |" << endl;
    cout << "|  [4] Commercial Sector        (Malls & Products)       |" << endl;
    cout << "|  [5] Public Facilities        (Parks & Services)       |" << endl;
    cout << "|  [6] Population & Housing     (Citizens & Census)      |" << endl;
    cout << "+========================================================+" << endl;
    cout << "|  [V] Visualization Mode       (Interactive City Map)   |" << endl;
    cout << "|  [0] Exit Program                                      |" << endl;
    cout << "+========================================================+" << endl;
    cout << "\n>> TIP: All inputs are case-insensitive! (e.g., 'g-10' = 'G-10')" << endl;
    cout << ">> Enter your choice: ";
}

// Bus System Menu
void busSystemMenu() {
    int choice = -1;
    do {
        cout << "\n+=============================================+" << endl;
        cout << "|     TRANSPORT SECTOR MENU                   |" << endl;
        cout << "+=============================================+" << endl;
        cout << "  [1] Display All Bus Stops" << endl;
        cout << "  [2] Display All Buses" << endl;
        cout << "  [3] Display Route Network Summary" << endl;
        cout << "  [4] Add New Bus Stop (Manual)" << endl;
        cout << "  [5] Register New Bus with Route" << endl;
        cout << "  [6] Get Specific Bus Details" << endl;
        cout << "  [7] Find Shortest Path Between Stops" << endl;
        cout << "  [8] Update Bus Location" << endl;
        cout << "  [9] Track Specific Bus Location" << endl;
        cout << "  [10] Find Nearest Bus to My Location" << endl;
        cout << "  [11] Register School Bus" << endl;
        cout << "  [12] Connect Two Bus Stops (Manual)" << endl;
        cout << "  [13] [DEMO] Simulate Passenger Queue (Boarding)" << endl;
        cout << "  [0] <-- Back to Transport Menu" << endl;
        cout << "+---------------------------------------------+" << endl;
        cout << ">> Stats: " << transportSystem->getBusCount() << " buses, "
             << transportSystem->getStopCount() << " stops, "
             << transportSystem->getRouteEdgeCount() << " connections" << endl;
        cout << ">> Your choice: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Invalid input! Please enter a number." << endl;
            choice = -1;
            continue;
        }
        
        if (choice == 1) {
            transportSystem->displayAllStops();
        } else if (choice == 2) {
            transportSystem->displayAllBuses();
        } else if (choice == 3) {
            transportSystem->displayRouteSummary();
        } else if (choice == 4) {
            transportSystem->addBusStopManual();
        } else if (choice == 5) {
            transportSystem->registerBusManual();
        } else if (choice == 6) {
            char busNo[20];
            cout << "\n=== Bus Details Lookup ===" << endl;
            cout << ">> Enter bus number (e.g., B101): ";
            cin >> busNo;
            normalizeInput(busNo);
            transportSystem->getBusDetails(busNo);
        } else if (choice == 7) {
            char from[50], to[50];
            cout << "\n=== Shortest Path Finder ===" << endl;
            cout << ">> Available Stops: Stop1, Stop2, Stop3, Stop4, Stop5," << endl;
            cout << "   Stop6, Stop7, Stop8, Stop9, Stop10, Stop11" << endl;
            cout << "\n>> Enter START stop (e.g., stop1, STOP1): ";
            cin >> from;
            normalizeInput(from);
            cout << ">> Enter DESTINATION stop (e.g., stop5): ";
            cin >> to;
            normalizeInput(to);
            transportSystem->findShortestPath(from, to);
        } else if (choice == 8) {
            char busNo[20], stop[50];
            cout << "\n=== Bus Location Update ===" << endl;
            cout << ">> Enter bus number (e.g., b101): ";
            cin >> busNo;
            normalizeInput(busNo);
            cout << ">> Enter new stop location (e.g., stop3): ";
            cin >> stop;
            normalizeInput(stop);
            transportSystem->updateBusLocation(busNo, stop);
        } else if (choice == 9) {
            transportSystem->trackBusLocation();
        } else if (choice == 10) {
            transportSystem->findNearestBus();
        } else if (choice == 11) {
            char busID[20], schoolID[50];
            cout << "\n=== Register School Bus ===" << endl;
            cout << ">> Enter school bus ID: ";
            cin >> busID;
            cout << ">> Enter school ID: ";
            cin >> schoolID;
            normalizeInput(schoolID);
            if (!educationSystem->schoolExists(schoolID)) {
                cout << "\n[ERROR] School " << schoolID << " does not exist!" << endl;
                cout << "[TIP] Register the school first in Education Sector (Menu [2])" << endl;
            } else {
                transportSystem->registerSchoolBus(busID, schoolID);
                cout << "[OK] School bus registered!" << endl;
            }
        } else if (choice == 12) {
            transportSystem->connectStopsManual();
        } else if (choice == 13) {
            cout << "\n=== Passenger Queue Simulation (Circular Queue) ===" << endl;
            char stopID[50];
            cout << ">> Enter Stop ID to simulate boarding: ";
            cin >> stopID;
            normalizeInput(stopID);
            transportSystem->simulatePassengerBoarding(stopID);
        } else if (choice != 0) {
            cout << "[ERROR] Invalid choice!" << endl;
        }
    } while (choice != 0);
}

// Transport menu with submenu
void transportMenu() {
    int choice = -1;
    do {
        cout << "\n+========================================================+" << endl;
        cout << "|   TRANSPORT & ROUTES SYSTEM                            |" << endl;
        cout << "+========================================================+" << endl;
        cout << "|  Select Transport Type:                                |" << endl;
        cout << "+--------------------------------------------------------+" << endl;
        cout << "  [1] Bus System        (Stops, Routes, Schedules)       " << endl;
        cout << "  [2] Airport System    (Airports, Flights)              " << endl;
        cout << "  [3] Railway System    (Stations, Trains)               " << endl;
        cout << "  [0] <-- Back to Main Menu                              " << endl;
        cout << "+========================================================+" << endl;
        cout << ">> Your choice: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            choice = -1;
            continue;
        }
        
        if (choice == 1) {
            busSystemMenu();
        } else if (choice == 2) {
            airportMenu();
        } else if (choice == 3) {
            railwayMenu();
        } else if (choice != 0) {
            cout << "[ERROR] Invalid choice!" << endl;
        }
    } while (choice != 0);
}

// Education menu
void educationMenu() {
    int choice = -1;
    do {
        cout << "\n+--------------------------------------------+" << endl;
        cout << "|   EDUCATION SECTOR MENU                    |" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << "  [1] Display All Schools Summary" << endl;
        cout << "  [2] Register New School (Manual)" << endl;
        cout << "  [3] Display Specific School Details" << endl;
        cout << "  [4] Add Department to School" << endl;
        cout << "  [5] Add Class to Department" << endl;
        cout << "  [6] Register Faculty Member" << endl;
        cout << "  [7] Register Student" << endl;
        cout << "  [8] Search Schools by Subject Offered" << endl;
        cout << "  [9] View School Rankings" << endl;
        cout << "  [10] Find Nearest School" << endl;
        cout << "  [11] Display All Students" << endl;
        cout << "  [0] <-- Back to Main Menu" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << ">> Stats: " << educationSystem->getSchoolCount() << " schools, "
             << educationSystem->getFacultyCount() << " faculty, "
             << educationSystem->getStudentCount() << " students" << endl;
        cout << ">> Your choice: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Invalid input! Please enter a number." << endl;
            choice = -1;
            continue;
        }
        
        if (choice == 0) {
            break;
        } else if (choice == 1) {
            educationSystem->displayAllSchools();
        } else if (choice == 2) {
            educationSystem->registerSchoolManual();
        } else if (choice == 3) {
            char schoolID[50];
            cout << "\n=== School Details ===" << endl;
            cout << ">> Enter school ID (S01-S05 or SCHxxxx): ";
            cin >> schoolID;
            normalizeInput(schoolID);
            educationSystem->displaySchool(schoolID);
        } else if (choice == 4) {
            char schoolID[50], deptName[100];
            cout << "\n=== Add Department ===" << endl;
            cout << ">> Enter school ID: ";
            cin >> schoolID;
            normalizeInput(schoolID);
            cin.ignore();
            cout << ">> Enter department name: ";
            cin.getline(deptName, 100);
            educationSystem->addDepartment(schoolID, deptName);
        } else if (choice == 5) {
            char schoolID[50], deptName[100], className[50];
            cout << "\n=== Add Class ===" << endl;
            cout << ">> Enter school ID: ";
            cin >> schoolID;
            normalizeInput(schoolID);
            cin.ignore();
            cout << ">> Enter department name: ";
            cin.getline(deptName, 100);
            cout << ">> Enter class name: ";
            cin.getline(className, 50);
            educationSystem->addClass(schoolID, deptName, className);
        } else if (choice == 6) {
            char id[50], name[100], dept[100], subject[100];
            cout << "\n=== Register Faculty ===" << endl;
            cout << ">> Enter faculty ID: ";
            cin >> id;
            cin.ignore();
            cout << ">> Enter name: ";
            cin.getline(name, 100);
            cout << ">> Enter department: ";
            cin.getline(dept, 100);
            cout << ">> Enter subject: ";
            cin.getline(subject, 100);
            educationSystem->registerFaculty(id, name, dept, subject);
        } else if (choice == 7) {
            educationSystem->registerStudentManual();
        } else if (choice == 8) {
            educationSystem->searchSchoolsBySubject();
        } else if (choice == 9) {
            educationSystem->displaySchoolRankings();
        } else if (choice == 10) {
            educationSystem->findNearestSchool();
        } else if (choice == 11) {
            educationSystem->displayAllStudents();
        } else {
            cout << "[ERROR] Invalid choice!" << endl;
        }
    } while (choice != 0);
}

// Medical menu
void medicalMenu() {
    int choice = -1;
    do {
        cout << "\n+--------------------------------------------+" << endl;
        cout << "|   MEDICAL SECTOR MENU                      |" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << "  [1] Display Medical System Summary" << endl;
        cout << "  [2] Display All Hospitals (Detailed)" << endl;
        cout << "  [3] Display All Pharmacies" << endl;
        cout << "  [4] Display All Medicines" << endl;
        cout << "  [5] Register New Hospital (Manual)" << endl;
        cout << "  [6] Register New Pharmacy (Manual)" << endl;
        cout << "  [7] Add Medicine to Pharmacy" << endl;
        cout << "  [8] Display Specific Hospital Details" << endl;
        cout << "  [9] Search Medicine by Name" << endl;
        cout << "  [10] Search Medicine by Formula" << endl;
        cout << "  [11] Get Top Emergency Hospital (Most Beds)" << endl;
        cout << "  [12] Find Nearest Hospital" << endl;
        cout << "  [13] Find Nearest Pharmacy" << endl;
        cout << "  [14] Register Patient" << endl;
        cout << "  [15] Register Doctor" << endl;
        cout << "  [16] Display All Doctors" << endl;
        cout << "  [17] Display All Patients" << endl;
        cout << "  [0] <-- Back to Main Menu" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << ">> Stats: " << medicalSystem->getHospitalCount() << " hospitals, "
             << medicalSystem->getPharmacyCount() << " pharmacies, "
             << medicalSystem->getMedicineCount() << " medicines" << endl;
        cout << ">> Your choice: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Invalid input! Please enter a number." << endl;
            choice = -1;
            continue;
        }
        
        if (choice == 1) {
            medicalSystem->displayAllSummary();
        } else if (choice == 2) {
            medicalSystem->displayAllHospitals();
        } else if (choice == 3) {
            medicalSystem->displayAllPharmacies();
        } else if (choice == 4) {
            medicalSystem->displayAllMedicines();
        } else if (choice == 5) {
            medicalSystem->registerHospitalManual();
            const char* sector = medicalSystem->getLastRegisteredSector();
            if (sector != nullptr && sector[0] != '\0') {
                populationSystem->autoRegisterSector(sector);
            }
        } else if (choice == 6) {
            medicalSystem->registerPharmacyManual();
            const char* sector = medicalSystem->getLastRegisteredSector();
            if (sector != nullptr && sector[0] != '\0') {
                populationSystem->autoRegisterSector(sector);
            }
        } else if (choice == 7) {
            medicalSystem->addMedicineManual();
        } else if (choice == 8) {
            char hospitalID[50];
            cout << "\n=== Hospital Details ===" << endl;
            cout << ">> Enter hospital ID (H01-H05 or HSPxxxx): ";
            cin >> hospitalID;
            medicalSystem->displayHospital(hospitalID);
        } else if (choice == 9) {
            char medicineName[100];
            cout << "\n=== Medicine Search ===" << endl;
            cout << ">> Enter medicine name: ";
            cin.ignore();
            cin.getline(medicineName, 100);
            medicalSystem->searchMedicine(medicineName);
        } else if (choice == 10) {
            medicalSystem->searchMedicineByFormula();
        } else if (choice == 11) {
            medicalSystem->getTopEmergencyHospital();
        } else if (choice == 12) {
            medicalSystem->findNearestHospital();
        } else if (choice == 13) {
            medicalSystem->findNearestPharmacy();
        } else if (choice == 14) {
            medicalSystem->registerPatientManual();
        } else if (choice == 15) {
            medicalSystem->registerDoctorManual();
        } else if (choice == 16) {
            medicalSystem->displayAllDoctors();
        } else if (choice == 17) {
            medicalSystem->displayAllPatients();
        } else if (choice != 0) {
            cout << "[ERROR] Invalid choice!" << endl;
        }
    } while (choice != 0);
}

// Population menu
void populationMenu() {
    int choice = -1;
    do {
        cout << "\n+--------------------------------------------+" << endl;
        cout << "|   POPULATION & HOUSING MENU                |" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << "  [1] View Sector Hierarchy Tree" << endl;
        cout << "  [2] Display All Citizens (Detailed)" << endl;
        cout << "  [3] Register New Sector (Dynamic)" << endl;
        cout << "  [4] Add Street to Sector" << endl;
        cout << "  [5] Register Family Member by CNIC" << endl;
        cout << "  [6] Lookup Citizen by CNIC" << endl;
        cout << "  [7] Update Citizen Address" << endl;
        cout << "  [8] Generate Age Distribution Report" << endl;
        cout << "  [9] Generate Occupation Report" << endl;
        cout << "  [10] Generate Population Heatmap" << endl;
        cout << "  [11] [DEMO] View Family Tree" << endl;
        cout << "  [0] <-- Back to Main Menu" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << ">> Total Population: " << populationSystem->getTotalPopulation() << endl;
        cout << ">> Your choice: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Invalid input! Please enter a number." << endl;
            choice = -1;
            continue;
        }
        
        if (choice == 1) {
            populationSystem->displaySectorHierarchy();
        } else if (choice == 2) {
            populationSystem->displayAllCitizens();
        } else if (choice == 3) {
            char sectorName[100];
            cout << "\n=== Register New Sector ===" << endl;
            cout << ">> Enter sector name (e.g., G-10, F-8): ";
            cin >> sectorName;
            populationSystem->addSector(sectorName);
            cout << "[OK] Sector registered!" << endl;
        } else if (choice == 4) {
            char sector[50], street[100];
            cout << "\n=== Add Street to Sector ===" << endl;
            cout << ">> Enter sector name: ";
            cin >> sector;
            cin.ignore();
            cout << ">> Enter street name: ";
            cin.getline(street, 100);
            populationSystem->addStreet(sector, street);
        } else if (choice == 5) {
            populationSystem->registerFamilyManual();
        } else if (choice == 6) {
            populationSystem->lookupByCNIC();
        } else if (choice == 7) {
            populationSystem->updateCitizenAddressManual();
        } else if (choice == 8) {
            populationSystem->generateAgeDistribution();
        } else if (choice == 9) {
            populationSystem->generateOccupationReport();
        } else if (choice == 10) {
            populationSystem->generatePopulationHeatmap();
        } else if (choice == 11) {
            char headCNIC[20];
            cout << "\n=== Family Tree Visualization ===" << endl;
            cout << ">> Enter Head of Family CNIC: ";
            cin >> headCNIC;
            populationSystem->displayFamilyTree(headCNIC);
        }
    } while (choice != 0);
}

// Commercial menu
void commercialMenu() {
    int choice = -1;
    do {
        cout << "\n+--------------------------------------------+" << endl;
        cout << "|   COMMERCIAL SECTOR MENU                   |" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << "  [1] Display Commercial System Summary" << endl;
        cout << "  [2] Display All Malls (Detailed)" << endl;
        cout << "  [3] Display All Products" << endl;
        cout << "  [4] Register New Shopping Mall (Manual)" << endl;
        cout << "  [5] Add Product to Mall (Manual)" << endl;
        cout << "  [6] Search Products by Category" << endl;
        cout << "  [7] Display Specific Mall Details" << endl;
        cout << "  [8] Find Nearest Mall" << endl;
        cout << "  [0] <-- Back to Main Menu" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << ">> Stats: " << commercialSystem->getMallCount() << " malls, "
             << commercialSystem->getProductCount() << " products" << endl;
        cout << ">> Your choice: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Invalid input! Please enter a number." << endl;
            choice = -1;
            continue;
        }
        
        if (choice == 1) {
            commercialSystem->displayAllSummary();
        } else if (choice == 2) {
            commercialSystem->displayAllMalls();
        } else if (choice == 3) {
            commercialSystem->displayAllProducts();
        } else if (choice == 4) {
            commercialSystem->registerMallManual();
            const char* sector = commercialSystem->getLastRegisteredSector();
            if (sector != nullptr && sector[0] != '\0') {
                populationSystem->autoRegisterSector(sector);
            }
        } else if (choice == 5) {
            commercialSystem->addProductManual();
        } else if (choice == 6) {
            commercialSystem->searchByCategory();
        } else if (choice == 7) {
            char mallID[50];
            cout << "\n=== Mall Details ===" << endl;
            cout << ">> Enter mall ID: ";
            cin >> mallID;
            commercialSystem->displayMall(mallID);
        } else if (choice == 8) {
            commercialSystem->findNearestMall();
        }
    } while (choice != 0);
}

// Public facilities menu
void publicFacilitiesMenu() {
    int choice = -1;
    do {
        cout << "\n+--------------------------------------------+" << endl;
        cout << "|   PUBLIC FACILITIES MENU                   |" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << "  [1] Display All Facilities Summary" << endl;
        cout << "  [2] Display All Facilities (Detailed)" << endl;
        cout << "  [3] Register New Facility (Manual)" << endl;
        cout << "  [4] Display Facilities by Type" << endl;
        cout << "  [5] Find Nearest Facility (by Type)" << endl;
        cout << "  [0] <-- Back to Main Menu" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << ">> Total Facilities: " << publicFacilities->getFacilityCount() << endl;
        cout << ">> Your choice: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Invalid input! Please enter a number." << endl;
            choice = -1;
            continue;
        }
        
        if (choice == 1) {
            publicFacilities->displayAllSummary();
        } else if (choice == 2) {
            publicFacilities->displayAllFacilities();
        } else if (choice == 3) {
            publicFacilities->registerFacilityManual();
            const char* sector = publicFacilities->getLastRegisteredSector();
            if (sector != nullptr && sector[0] != '\0') {
                populationSystem->autoRegisterSector(sector);
            }
        } else if (choice == 4) {
            publicFacilities->displayByType();
        } else if (choice == 5) {
            publicFacilities->findNearestFacilityByType();
        }
    } while (choice != 0);
}

int main() {
    // WELCOME SCREEN
    cout << "\n\n";
    cout << "+==============================================================+" << endl;
    cout << "|                                                              |" << endl;
    cout << "|               ****  *   *   *   ****  *****                  |" << endl;
    cout << "|              *      ** **  * *  *   *   *                    |" << endl;
    cout << "|               ***   * * * ***** ****    *                    |" << endl;
    cout << "|                  *  *   * *   * *   *   *                    |" << endl;
    cout << "|              ****   *   * *   * *   *   *                    |" << endl;
    cout << "|                                                              |" << endl;
    cout << "|            ** CITY MANAGEMENT SYSTEM **                      |" << endl;
    cout << "|               ~ ISBGlyph - Islamabad ~                         |" << endl;
    cout << "|                                                              |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "|             Data Structures Semester Project                 |" << endl;
    cout << "|     Muhammad Dyen Asif | Ali Ather | Muhammad Saad           |" << endl;
    cout << "|                                                              |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "\n";
    
    cout << "+-------------------------------------+" << endl;
    cout << "|   ** QUICK START GUIDE **           |" << endl;
    cout << "+-------------------------------------+" << endl;
    cout << "[OK] Input is CASE-INSENSITIVE -> Type 'g-10' or 'G-10'" << endl;
    cout << "[OK] Format hints shown for every input" << endl;
    cout << "[OK] Sample data already loaded from CSV files" << endl;
    cout << "\n>> Now loading city data..." << endl;
    cout << "=================================================" << endl;
    
    // Initialize all sector systems
    populationSystem = new PopulationSystem();
    educationSystem = new EducationSystem();
    transportSystem = new TransportSystem();
    airportSystem = new AirportSystem();
    railwaySystem = new RailwaySystem();
    medicalSystem = new MedicalSystem();
    commercialSystem = new CommercialSystem();
    publicFacilities = new PublicFacilitySystem();
    
    cout << "\n>> Loading data from CSV files..." << endl;
    
    // IMPORTANT: Load entities with sectors FIRST (hospitals, schools, pharmacies)
    // so that bus stops can be distributed across those sectors
    loadHospitals();
    loadPharmacies();
    loadSchools();
    loadPopulation();
    
    // NOW load bus stops (will distribute across sectors from above entities)
    loadStops();
    loadBuses();
    
    cout << "\n[OK] Data loaded successfully!" << endl;
    cout << "=================================================" << endl;
    cout << "\n>> Real-Time System Statistics:" << endl;
    cout << "   * Transport: Bus Stops: " << transportSystem->getStopCount() 
         << " | Buses: " << transportSystem->getBusCount() 
         << " | Route Connections: " << transportSystem->getRouteEdgeCount() << endl;
    cout << "   * Transport: Airports: " << airportSystem->getAirportCount()
         << " | Flights: " << airportSystem->getFlightCount()
         << " | Connections: " << airportSystem->getRouteEdgeCount() << endl;
    cout << "   * Transport: Railway Stations: " << railwaySystem->getStationCount()
         << " | Trains: " << railwaySystem->getTrainCount()
         << " | Connections: " << railwaySystem->getRouteEdgeCount() << endl;
    cout << "   * Education: Schools: " << educationSystem->getSchoolCount() 
         << " | Faculty: " << educationSystem->getFacultyCount() 
         << " | Students: " << educationSystem->getStudentCount() << endl;
    cout << "   * Medical: Hospitals: " << medicalSystem->getHospitalCount() 
         << " | Doctors: " << medicalSystem->getDoctorCount()
         << " | Patients: " << medicalSystem->getPatientCount() << endl;
    cout << "   * Medical: Pharmacies: " << medicalSystem->getPharmacyCount()
         << " | Medicines: " << medicalSystem->getMedicineCount() << endl;
    cout << "   * Population: Citizens Registered: " << populationSystem->getTotalPopulation() << endl;
    cout << "=================================================" << endl;
    cout << "\n>> Ready! Press ENTER to continue to main menu...";
    cin.get();
    
    // Main menu loop
    char choice = -1;
    do {
        displayMainMenu();
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Invalid input!" << endl;
            choice = -1;
            continue;
        }
        
        if (choice == '1') {
            transportMenu();
        } else if (choice == '2') {
            educationMenu();
        } else if (choice == '3') {
            medicalMenu();
        } else if (choice == '4') {
            commercialMenu();
        } else if (choice == '5') {
            publicFacilitiesMenu();
        } else if (choice == '6') {
            populationMenu();
        } else if (choice == 'V' || choice == 'v') {
            // VISUALIZATION MODE
            if (initVisualization()) {
                cout << "\n>> Switching to Visualization Mode..." << endl;
                
                // CRITICAL: Reload all data to reflect any CLI changes
                viz->reloadAllData(populationSystem, transportSystem, 
                                  medicalSystem, educationSystem,
                                  commercialSystem, publicFacilities,
                                  airportSystem, railwaySystem);
                
                cout << "   WASD: Move | Q/E: Zoom | 1-9,0,-,=: Toggle layers | R: Reset | ESC: Return to CLI\n" << endl;
                viz->run();
                cout << "\n>> Returned to CLI Mode" << endl;
            }
        } else if (choice == '0') {
            cout << "\n+========================================================+" << endl;
            cout << "|  Thank you for using ISBGlyph!                          |" << endl;
            cout << "|  Goodbye!                                               |" << endl;
            cout << "+=========================================================+" << endl;
        } else {
            cout << "\n[ERROR] Invalid choice! Please select 0-6 or V." << endl;
        }
    } while (choice != '0');
    
    // cleanup
    if (viz != nullptr) delete viz;
    delete transportSystem;
    delete educationSystem;
    delete medicalSystem;
    delete commercialSystem;
    delete publicFacilities;
    delete populationSystem;
    delete airportSystem;
    delete railwaySystem;
    
    cout << "Program ended successfully!" << endl;
    return 0;
}
