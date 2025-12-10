#ifndef MEDICALSECTOR_H
#define MEDICALSECTOR_H

#include "HashTable.h"
#include "MaxHeap.h"  // Changed to MaxHeap for emergency bed ranking (highest first)

// Forward declaration for global access
class PopulationSystem;
extern PopulationSystem* populationSystem;

#include "Utils.h"
#include <iostream>
using namespace std;

// structures for medical sector
struct Doctor {
    char doctorID[50];
    char name[100];
    char cnic[20];           // Link to population
    char hospitalID[50];     // Which hospital
    char specialization[100];
    
    Doctor() {
        doctorID[0] = '\0';
        name[0] = '\0';
        cnic[0] = '\0';
        hospitalID[0] = '\0';
        specialization[0] = '\0';
    }
};

struct Patient {
    char patientID[50];
    char name[100];
    char cnic[20];           // Link to population
    char hospitalID[50];     // Which hospital
    int age;
    char condition[200];
    
    Patient() {
        patientID[0] = '\0';
        name[0] = '\0';
        cnic[0] = '\0';
        hospitalID[0] = '\0';
        age = 0;
        condition[0] = '\0';
    }
};

struct Medicine {
    char medicineName[100];
    char formula[100];
    double price;
    char pharmacyID[50];
    
    Medicine() {
        medicineName[0] = '\0';
        formula[0] = '\0';
        price = 0.0;
        pharmacyID[0] = '\0';
    }
};

struct Hospital {
    char hospitalID[50];
    char name[100];
    char sector[50];
    int emergencyBeds;
    char specialization[200];
    
    Hospital() {
        hospitalID[0] = '\0';
        name[0] = '\0';
        sector[0] = '\0';
        emergencyBeds = 0;
        specialization[0] = '\0';
    }
};

struct Pharmacy {
    char pharmacyID[50];
    char name[100];
    char sector[50];
    
    Pharmacy() {
        pharmacyID[0] = '\0';
        name[0] = '\0';
        sector[0] = '\0';
    }
};

// medical system class
class MedicalSystem {
private:
    HashTable* hospitalTable;
    HashTable* doctorTable;
    HashTable* patientTable;
    HashTable* medicineTable;
    HashTable* pharmacyTable;
    MaxHeap* emergencyBedHeap;  // MaxHeap for highest bed count priority
    
    // Counters
    int hospitalCount;
    int doctorCount;
    int patientCount;
    int medicineCount;
    int pharmacyCount;
    char lastRegisteredSector[50];  // storing the last sector we used

public:
    MedicalSystem() {
        hospitalTable = new HashTable(50);
        doctorTable = new HashTable(100);
        patientTable = new HashTable(500);
        medicineTable = new HashTable(200);
        pharmacyTable = new HashTable(50);
        emergencyBedHeap = new MaxHeap(50);  // Use MaxHeap
        hospitalCount = 0;
        doctorCount = 0;
        patientCount = 0;
        medicineCount = 0;
        pharmacyCount = 0;
        lastRegisteredSector[0] = '\0';
    }
    
    // register hospital
    void registerHospital(const char* hospitalID, const char* name, const char* sector,
                         int beds, const char* specialization) {
        Hospital* hospital = new Hospital();
        stringCopyN(hospital->hospitalID, hospitalID, 50);
        stringCopyN(hospital->name, name, 100);
        stringCopyN(hospital->sector, sector, 50);
        hospital->emergencyBeds = beds;
        stringCopyN(hospital->specialization, specialization, 200);
        
        hospitalTable->insert(hospitalID, hospital);
        emergencyBedHeap->insert(hospital, beds);  // Positive value for MaxHeap
        hospitalCount++;
        
        // Hospital registered (quiet mode)
    }
    
    // register doctor
    void registerDoctor(const char* doctorID, const char* name, const char* specialization) {
        Doctor* doctor = new Doctor();
        stringCopyN(doctor->doctorID, doctorID, 50);
        stringCopyN(doctor->name, name, 100);
        stringCopyN(doctor->specialization, specialization, 100);
        
        doctorTable->insert(doctorID, doctor);
        doctorCount++;
        // cout << "Registered doctor: " << name << " (" << specialization << ")" << endl;
    }
    
    // add patient
    void addPatient(const char* patientID, const char* name, const char* condition) {
        Patient* patient = new Patient();
        stringCopyN(patient->patientID, patientID, 50);
        stringCopyN(patient->name, name, 100);
        stringCopyN(patient->condition, condition, 200);
        
        patientTable->insert(patientID, patient);
        patientCount++;
        // cout << "Added patient: " << name << endl;
    }
    
    // registering pharmacy
    void registerPharmacy(const char* pharmacyID, const char* name, const char* sector) {
        Pharmacy* pharmacy = new Pharmacy();
        stringCopyN(pharmacy->pharmacyID, pharmacyID, 50);
        stringCopyN(pharmacy->name, name, 100);
        stringCopyN(pharmacy->sector, sector, 50);
        
        pharmacyTable->insert(pharmacyID, pharmacy);
        pharmacyCount++;
        
        // Pharmacy registered (quiet mode)
    }
    
    // adding medicine
    void addMedicine(const char* medicineName, const char* formula, 
                    double price, const char* pharmacyID) {
        Medicine* medicine = new Medicine();
        stringCopyN(medicine->medicineName, medicineName, 100);
        stringCopyN(medicine->formula, formula, 100);
        medicine->price = price;
        stringCopyN(medicine->pharmacyID, pharmacyID, 50);
        
        medicineTable->insert(medicineName, medicine);
        medicineCount++;
        
        // Medicine added (quiet mode)
    }
    
    // searching for medicine
    void searchMedicine(const char* medicineName) {
        Medicine* med = (Medicine*)medicineTable->search(medicineName);
        if (med == nullptr) {
            cout << "Medicine not found!" << endl;
            return;
        }
        
        cout << "\n--- Medicine Details ---" << endl;
        cout << "Name: " << med->medicineName << endl;
        cout << "Formula: " << med->formula << endl;
        cout << "Price: Rs. " << med->price << endl;
        cout << "Pharmacy ID: " << med->pharmacyID << endl;
    }
    
    // displaying hospital
    void displayHospital(const char* hospitalID) {
        Hospital* hosp = (Hospital*)hospitalTable->search(hospitalID);
        if (hosp == nullptr) {
            cout << "Hospital not found!" << endl;
            return;
        }
        
        cout << "\n--- Hospital Details ---" << endl;
        cout << "Hospital ID: " << hosp->hospitalID << endl;
        cout << "Name: " << hosp->name << endl;
        cout << "Sector: " << hosp->sector << endl;
        cout << "Emergency Beds: " << hosp->emergencyBeds << endl;
        cout << "Specialization: " << hosp->specialization << endl;
    }
    
    // get top emergency hospital (most beds available)
    void getTopEmergencyHospital() {
        if (emergencyBedHeap->isEmpty()) {
            cout << "No hospitals registered!" << endl;
            return;
        }
        
        Hospital* topHospital = (Hospital*)emergencyBedHeap->getMax();  // getMax for MaxHeap
        cout << "\n--- Hospital with Most Emergency Beds ---" << endl;
        cout << "Name: " << topHospital->name << endl;
        cout << "Emergency Beds: " << topHospital->emergencyBeds << endl;
    }
    
    // grabbing stats
    int getHospitalCount() { return hospitalCount; }
    int getPharmacyCount() { return pharmacyCount; }
    int getMedicineCount() { return medicineCount; }
    int getDoctorCount() { return doctorCount; }
    int getPatientCount() { return patientCount; }
    const char* getLastRegisteredSector() const { return lastRegisteredSector; }
    
    // Display all summary
    void displayAllSummary() {
        cout << "\n========== MEDICAL SECTOR SUMMARY ==========" << endl;
        cout << "Total Hospitals: " << hospitalCount << endl;
        cout << "Total Pharmacies: " << pharmacyCount << endl;
        cout << "Total Medicines: " << medicineCount << endl;
        cout << "Total Doctors: " << doctorCount << endl;
        cout << "Total Patients: " << patientCount << endl;
        cout << "\nUse specific display options for detailed info" << endl;
        cout << "==========================================" << endl;
    }
    
    // Display all hospitals
    void displayAllHospitals() {
        cout << "\n========== ALL HOSPITALS ==========" << endl;
        
        char** hospKeys = new char*[100];
        int count;
        hospitalTable->getAllKeys(hospKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No hospitals registered yet." << endl;
            delete[] hospKeys;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            Hospital* hosp = (Hospital*)hospitalTable->search(hospKeys[i]);
            if (hosp != nullptr) {
                cout << "\n[" << (i+1) << "] " << hosp->name << endl;
                cout << "    ID: " << hosp->hospitalID << endl;
                cout << "    Sector: " << hosp->sector << endl;
                cout << "    Emergency Beds: " << hosp->emergencyBeds << endl;
                cout << "    Specializations: " << hosp->specialization << endl;
            }
        }
        
        delete[] hospKeys;
        cout << "\n==================================" << endl;
    }
    
    // Display all pharmacies
    void displayAllPharmacies() {
        cout << "\n========== ALL PHARMACIES ==========" << endl;
        
        char** pharmKeys = new char*[100];
        int count;
        pharmacyTable->getAllKeys(pharmKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No pharmacies registered yet." << endl;
            delete[] pharmKeys;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            Pharmacy* pharm = (Pharmacy*)pharmacyTable->search(pharmKeys[i]);
            if (pharm != nullptr) {
                cout << "\n[" << (i+1) << "] " << pharm->name << endl;
                cout << "    ID: " << pharm->pharmacyID << endl;
                cout << "    Sector: " << pharm->sector << endl;
            }
        }
        
        delete[] pharmKeys;
        cout << "\n====================================" << endl;
    }
    
    // Display all medicines
    void displayAllMedicines() {
        cout << "\n========== ALL MEDICINES ==========" << endl;
        
        char** medKeys = new char*[500];
        int count;
        medicineTable->getAllKeys(medKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No medicines registered yet." << endl;
            delete[] medKeys;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            Medicine* med = (Medicine*)medicineTable->search(medKeys[i]);
            if (med != nullptr) {
                cout << "\n[" << (i+1) << "] " << med->medicineName << endl;
                cout << "    Formula: " << med->formula << endl;
                cout << "    Price: $" << med->price << endl;
                cout << "    Pharmacy: " << med->pharmacyID << endl;
            }
        }
        
        delete[] medKeys;
        cout << "\n===================================" << endl;
    }
    
    // ========== MANUAL REGISTRATION METHODS ==========
    
    // Register hospital manually
    void registerHospitalManual() {
        char hospitalID[50], name[100], sector[50], specialization[100];
        int emergencyBeds;
        
        cout << "\n=== Register New Hospital ===" << endl;
        generateUniqueID("HSP", hospitalID);
        cout << "Generated Hospital ID: " << hospitalID << endl;
        
        cout << "Enter hospital name: ";
        cin.ignore();
        cin.getline(name, 100);
        
        cout << "Enter sector (e.g., G-10, F-8): ";
        cin >> sector;
        
        // auto-registering sector to tree (cross-sector integration)
        populationSystem->autoRegisterSector(sector);
        
        cout << "Enter specialization: ";
        cin.ignore();
        cin.getline(specialization, 100);
        
        cout << "Enter number of emergency beds: ";
        cin >> emergencyBeds;
        
        registerHospital(hospitalID, name, sector, emergencyBeds, specialization);
        cout << "\n[OK] Hospital registered successfully!" << endl;
    }
    
    // Register pharmacy manually
    void registerPharmacyManual() {
        char pharmacyID[50], name[100], sector[50];
        int is24_7;
        
        cout << "\n=== Register New Pharmacy ===" << endl;
        generateUniqueID("PHM", pharmacyID);
        cout << "Generated Pharmacy ID: " << pharmacyID << endl;
        
        cout << "Enter pharmacy name: ";
        cin.ignore();
        cin.getline(name, 100);
        
        cout << "Enter sector (e.g., G-10, F-8): ";
        cin >> sector;
        
        // auto-registering sector to tree (cross-sector integration)
        populationSystem->autoRegisterSector(sector);
        
        cout << "Enter 24/7 service (1=Yes, 0=No): ";
        cin >> is24_7;
        
        registerPharmacy(pharmacyID, name, sector);
        cout << "\n[OK] Pharmacy registered successfully!" << endl;
    }
    
    // Add medicine to pharmacy
    void addMedicineManual() {
        char medicineName[100], formula[100], pharmacyID[50];
        double price;
        
        cout << "\n=== Add Medicine ===" << endl;
        cout << "Enter medicine name: ";
        cin.ignore();
        cin.getline(medicineName, 100);
        
        cout << "Enter chemical formula: ";
        cin.getline(formula, 100);
        
        cout << "Enter price: ";
        cin >> price;
        
        cout << "Enter pharmacy ID: ";
        cin >> pharmacyID;
        normalizeInput(pharmacyID);
        
        // making sure Check if pharmacy exists
        Pharmacy* pharm = (Pharmacy*)pharmacyTable->search(pharmacyID);
        if (pharm == nullptr) {
            cout << "\n[ERROR] Pharmacy " << pharmacyID << " does not exist!" << endl;
            cout << "[TIP] Register the pharmacy first, or check available pharmacies." << endl;
            return;
        }
        
        addMedicine(medicineName, formula, price, pharmacyID);
        cout << "\n[OK] Medicine added successfully!" << endl;
    }
    
    // Register doctor manually (WITH CNIC AND POPULATION INTEGRATION)
    void registerDoctorManual() {
        char doctorID[50], name[100], cnic[20], hospitalID[50], specialization[100];
        char sector[50], street[50], house[20], gender[10];
        int age;
        
        cout << "\n=== Register New Doctor ===" << endl;
        
        cout << "Enter CNIC: ";
        cin >> cnic;
        
        // making sure Check if CNIC already exists
        if (populationSystem->cnicExists(cnic)) {
            cout << "\n[ERROR] Person with CNIC " << cnic << " already registered!" << endl;
            cout << "[TIP] Use Population Lookup to view existing person" << endl;
            return;
        }
        
        cin.ignore();
        cout << "Enter name: ";
        cin.getline(name, 100);
        
        cout << "Enter hospital ID: ";
        cin >> hospitalID;
        normalizeInput(hospitalID);
        
        // making sure Check if hospital exists
        if (!hospitalTable->contains(hospitalID)) {
            cout << "\n[ERROR] Hospital " << hospitalID << " does not exist!" << endl;
            cout << "[TIP] Register the hospital first (Option [5])" << endl;
            return;
        }
        
        cout << "Enter specialization: ";
        cin.ignore();
        cin.getline(specialization, 100);
        
        cout << "Enter gender (Male/Female): ";
        cin >> gender;
        
        cout << "Enter age: ";
        cin >> age;
        
        cout << "Enter sector (doctor's home): ";
        cin >> sector;
        
        cout << "Enter street name: ";
        cin.ignore();
        cin.getline(street, 50);
        
        cout << "Enter house number: ";
        cin >> house;
        
        // Auto-register sector
        populationSystem->autoRegisterSector(sector);
        
        // Add to population first (as a Doctor)
        populationSystem->registerCitizen(cnic, name, age, sector, street, house, "Doctor", gender);
        
        // Then add to medical system
        generateUniqueID("DOC", doctorID);
        Doctor* doctor = new Doctor();
        stringCopyN(doctor->doctorID, doctorID, 50);
        stringCopyN(doctor->name, name, 100);
        stringCopyN(doctor->cnic, cnic, 20);
        stringCopyN(doctor->hospitalID, hospitalID, 50);
        stringCopyN(doctor->specialization, specialization, 100);
        
        doctorTable->insert(doctorID, doctor);
        
        cout << "\n[OK] Doctor registered successfully!" << endl;
        cout << "Doctor ID: " << doctorID << endl;
    }
    
    // Display all doctors
    void displayAllDoctors() {
        cout << "\n========== ALL DOCTORS ==========" << endl;
        
        char** keys = new char*[1000];
        int count;
        doctorTable->getAllKeys(keys, &count);
        
        if (count == 0) {
            cout << "[INFO] No doctors registered yet." << endl;
            cout << "====================================" << endl;
            delete[] keys;
            return;
        }
        
        cout << "Total Doctors: " << count << endl << endl;
        
        for (int i = 0; i < count; i++) {
            Doctor* doctor = (Doctor*)doctorTable->search(keys[i]);
            if (doctor != nullptr) {
                cout << "[" << (i+1) << "] " << doctor->name << endl;
                cout << "    ID: " << doctor->doctorID << endl;
                cout << "    CNIC: " << doctor->cnic << endl;
                cout << "    Hospital: " << doctor->hospitalID << endl;
                cout << "    Specialization: " << doctor->specialization << endl;
                cout << endl;
            }
        }
        
        delete[] keys;
        cout << "====================================" << endl;
    }
    
    // Register patient manually (WITH CNIC AND POPULATION INTEGRATION)
    void registerPatientManual() {
        char patientID[50], name[100], cnic[20], hospitalID[50], condition[200];
        char sector[50], street[50], house[20], gender[10], occupation[50];
        int age;
        
        cout << "\n=== Register New Patient ===" << endl;
        
        cout << "Enter patient CNIC: ";
        cin >> cnic;
        
        // Validate CNIC format
        if (!isValidCNIC(cnic)) {
            cout << "\n[ERROR] Invalid CNIC format!" << endl;
            cout << "[TIP] CNIC should be: XXXXX-XXXXXXX-X (5 digits-7 digits-1 digit)" << endl;
            return;
        }
        
        // Check if already exists
        if (populationSystem->cnicExists(cnic)) {
            cout << "\n[ERROR] Person with CNIC " << cnic << " already registered!" << endl;
            return;
        }
        
        cin.ignore();
        cout << "Enter name: ";
        cin.getline(name, 100);
        
        cout << "Enter age: ";
        cin >> age;
        
        // Validate age
        if (!isValidAge(age)) {
            cout << "\n[ERROR] Invalid age! Age must be between 0 and 120." << endl;
            return;
        }
        
        cout << "Enter hospital ID: ";
        cin >> hospitalID;
        normalizeInput(hospitalID);
        
        // making sure Check if hospital exists
        if (!hospitalTable->contains(hospitalID)) {
            cout << "\n[ERROR] Hospital " << hospitalID << " does not exist!" << endl;
            cout << "[TIP] Register the hospital first or view hospitals (Option [2])" << endl;
            return;
        }
        
        cout << "Enter medical condition: ";
        cin.ignore();
        cin.getline(condition, 200);
        
        cout << "Enter gender (Male/Female): ";
        cin >> gender;
        
        cout << "Enter sector (patient's home): ";
        cin >> sector;
        
        cout << "Enter street name: ";
        cin.ignore();
        cin.getline(street, 50);
        
        cout << "Enter house number: ";
        cin >> house;
        
        cout << "Enter occupation (e.g., Teacher, Engineer, Student): ";
        cin.ignore();
        cin.getline(occupation, 50);
        
        // Auto-register sector
        populationSystem->autoRegisterSector(sector);
        
        // Add to population first with actual occupation and gender
        populationSystem->registerCitizen(cnic, name, age, sector, street, house, occupation, gender);
        
        // Then add to medical system
        generateUniqueID("PAT", patientID);
        Patient* patient = new Patient();
        stringCopyN(patient->patientID, patientID, 50);
        stringCopyN(patient->name, name, 100);
        stringCopyN(patient->cnic, cnic, 20);
        stringCopyN(patient->hospitalID, hospitalID, 50);
        patient->age = age;
        stringCopyN(patient->condition, condition, 200);
        
        patientTable->insert(patientID, patient);
        
        cout << "\n[OK] Patient registered successfully!" << endl;
        cout << "Patient ID: " << patientID << endl;
    }
    
    // Display all patients
    void displayAllPatients() {
        cout << "\n========== ALL PATIENTS ==========" << endl;
        
        char** keys = new char*[1000];
        int count;
        patientTable->getAllKeys(keys, &count);
        
        if (count == 0) {
            cout << "[INFO] No patients registered yet." << endl;
            cout << "=====================================" << endl;
            delete[] keys;
            return;
        }
        
        cout << "Total Patients: " << count << endl << endl;
        
        for (int i = 0; i < count; i++) {
            Patient* patient = (Patient*)patientTable->search(keys[i]);
            if (patient != nullptr) {
                cout << "[" << (i+1) << "] " << patient->name << endl;
                cout << "    ID: " << patient->patientID << endl;
                cout << "    CNIC: " << patient->cnic << endl;
                cout << "    Hospital: " << patient->hospitalID << endl;
                cout << "    Age: " << patient->age << endl;
                cout << "    Condition: " << patient->condition << endl;
                cout << endl;
            }
        }
        
        delete[] keys;
        cout << "=====================================" << endl;
    }
    
    // Search medicine by formula
    void searchMedicineByFormula() {
        char formula[100];
        cout << "\n=== Search Medicine by Formula ===" << endl;
        cout << "Enter chemical formula: ";
        cin.ignore();
        cin.getline(formula, 100);
        toUpperCase(formula);
        
        // Get all medicine keys
        char** medKeys = new char*[500];
        int count;
        medicineTable->getAllKeys(medKeys, &count);
        
        bool found = false;
        cout << "\n--- Medicines with Formula " << formula << " ---" << endl;
        
        for (int i = 0; i < count; i++) {
            Medicine* med = (Medicine*)medicineTable->search(medKeys[i]);
            if (med != nullptr) {
                char upperFormula[100];
                stringCopy(upperFormula, med->formula);
                toUpperCase(upperFormula);
                
                if (stringCompare(upperFormula, formula) == 0) {
                    cout << "  - " << med->medicineName << endl;
                    cout << "    Price: $" << med->price << endl;
                    cout << "    Pharmacy: " << med->pharmacyID << endl;
                    found = true;
                }
            }
        }
        
        delete[] medKeys;
        
        if (!found) {
            cout << "[INFO] No medicines found with this formula." << endl;
        }
    }
    
    // finding nearest hospital - SECTOR-BASED
    void findNearestHospital() {
        char userSector[50];
        
        cout << "\n=== Find Nearest Hospital ===" << endl;
        cout << "Enter your sector (e.g., G-10): ";
        cin >> userSector;
        normalizeInput(userSector);
        
        char** keys = new char*[100];
        int count;
        hospitalTable->getAllKeys(keys, &count);
        
        bool found = false;
        cout << "\n--- Hospitals in Your Sector ---" << endl;
        
        for (int i = 0; i < count; i++) {
            Hospital* hospital = (Hospital*)hospitalTable->search(keys[i]);
            if (hospital != nullptr) {
                char hospitalSector[50];
                stringCopy(hospitalSector, hospital->sector);
                normalizeInput(hospitalSector);
                
                if (stringCompare(hospitalSector, userSector) == 0) {
                    cout << "  - " << hospital->name << endl;
                    cout << "    Emergency Beds: " << hospital->emergencyBeds << endl;
                    found = true;
                }
            }
        }
        
        delete[] keys;
        
        if (!found) {
            cout << "[INFO] No hospitals found in your sector." << endl;
            cout << "Try the Top Emergency Hospital option." << endl;
        }
    }
    
    // finding nearest pharmacy - SECTOR-BASED
    void findNearestPharmacy() {
        char userSector[50];
        
        cout << "\n=== Find Nearest Pharmacy ===" << endl;
        cout << "Enter your sector (e.g., G-10): ";
        cin >> userSector;
        normalizeInput(userSector);
        
        char** keys = new char*[100];
        int count;
        pharmacyTable->getAllKeys(keys, &count);
        
        bool found = false;
        cout << "\n--- Pharmacies in Your Sector ---" << endl;
        
        for (int i = 0; i < count; i++) {
            Pharmacy* pharmacy = (Pharmacy*)pharmacyTable->search(keys[i]);
            if (pharmacy != nullptr) {
                char pharmacySector[50];
                stringCopy(pharmacySector, pharmacy->sector);
                normalizeInput(pharmacySector);
                
                if (stringCompare(pharmacySector, userSector) == 0) {
                    cout << "  - " << pharmacy->name << endl;
                    found = true;
                }
            }
        }
        
        delete[] keys;
        
        if (!found) {
            cout << "[INFO] No pharmacies found in your sector." << endl;
        }
    }
    
    // ========== VISUALIZATION HELPER METHODS ==========
    
    // Get all hospital keys for visualization
    void getHospitalKeys(char** keys, int* count) {
        hospitalTable->getAllKeys(keys, count);
    }
    
    // Get hospital by ID
    Hospital* getHospital(const char* hospitalID) {
        return (Hospital*)hospitalTable->search(hospitalID);
    }
    
    // Pharmacy visualization helpers
    void getPharmacyKeys(char** keys, int* count) {
        pharmacyTable->getAllKeys(keys, count);
    }
    
    Pharmacy* getPharmacy(const char* pharmacyID) {
        return (Pharmacy*)pharmacyTable->search(pharmacyID);
    }
    
    ~MedicalSystem() {
        delete hospitalTable;
        delete doctorTable;
        delete patientTable;
        delete medicineTable;
        delete pharmacyTable;
        delete emergencyBedHeap;
    }
};

#endif
