#ifndef COMMERCIALSECTOR_H
#define COMMERCIALSECTOR_H

#include "HashTable.h"
#include "Utils.h"
#include <iostream>
using namespace std;

// Forward declaration
class PopulationSystem;
extern PopulationSystem* populationSystem;

struct Product {
    char productID[50];
    char name[100];
    char category[50];
    double price;
    
    Product() {
        productID[0] = '\0';
        name[0] = '\0';
        category[0] = '\0';
        price = 0.0;
    }
};

struct Mall {
    char mallID[50];
    char name[100];
    char sector[50];
    
    Mall() {
        mallID[0] = '\0';
        name[0] = '\0';
        sector[0] = '\0';
    }
};

// commercial system class
class CommercialSystem {
private:
    HashTable* mallTable;
    HashTable* productTable;
    HashTable* categoryIndex;  // category → LinkedList of productIDs
    int mallCount;
    int productCount;
    char lastRegisteredSector[50];  // storing the last sector we used for external access

public:
    CommercialSystem() {
        mallTable = new HashTable(50);
        productTable = new HashTable(500);
        categoryIndex = new HashTable(50);
        mallCount = 0;
        productCount = 0;
        lastRegisteredSector[0] = '\0';
    }
    
    // register mall
    void registerMall(const char* mallID, const char* name, const char* sector) {
        Mall* mall = new Mall();
        stringCopyN(mall->mallID, mallID, 50);
        stringCopyN(mall->name, name, 100);
        stringCopyN(mall->sector, sector, 50);
        
        mallTable->insert(mallID, mall);
        mallCount++;
        cout << "Registered mall: " << name << endl;
    }
    
    // Register mall manually
    void registerMallManual() {
        char mallID[50], name[100], sector[50];
        
        cout << "\n=== Register New Shopping Mall ===" << endl;
        
        generateUniqueID("MALL", mallID);
        cout << "Generated Mall ID: " << mallID << endl;
        
        cout << "Enter mall name: ";
        cin.ignore();
        cin.getline(name, 100);
        
        cout << "Enter sector (e.g., G-10, F-8): ";
        cin >> sector;
        stringCopy(lastRegisteredSector, sector);  // Store for external access
        
        // auto-registering sector to tree (cross-sector integration)
        populationSystem->autoRegisterSector(sector);
        
        registerMall(mallID, name, sector);
        cout << "\n[OK] Mall registered successfully!" << endl;
    }
    
    // add product to mall
    void addProduct(const char* productID, const char* name, const char* category, double price) {
        Product* product = new Product();
        stringCopyN(product->productID, productID, 50);
        stringCopyN(product->name, name, 100);
        stringCopyN(product->category, category, 50);
        product->price = price;
        
        productTable->insert(productID, product);
        productCount++;
        
        // Add to category index
        LinkedList* catList = (LinkedList*)categoryIndex->search(category);
        if (catList == nullptr) {
            catList = new LinkedList();
            categoryIndex->insert(category, catList);
        }
        catList->append(productID);
        
        cout << "Added product: " << name << endl;
    }
    
    // Add product manually
    void addProductManual() {
        char productID[50], name[100], category[50], mallID[50];
        double price;
        
        cout << "\n=== Add Product ===" << endl;
        
        generateUniqueID("PROD", productID);
        cout << "Generated Product ID: " << productID << endl;
        
        cout << "Enter product name: ";
        cin.ignore();
        cin.getline(name, 100);
        
        cout << "Enter category (e.g., Electronics, Clothing, Food): ";
        cin.getline(category, 50);
        
        cout << "Enter price: ";
        cin >> price;
        
        cout << "Enter mall ID (where this product is sold): ";
        cin >> mallID;
        normalizeInput(mallID);
        
        // making sure Check if mall exists
        Mall* mall = (Mall*)mallTable->search(mallID);
        if (mall == nullptr) {
            cout << "\n[ERROR] Mall " << mallID << " does not exist!" << endl;
            cout << "[TIP] Register the mall first, or check available malls." << endl;
            return;
        }
        
        addProduct(productID, name, category, price);
        cout << "\n[OK] Product added successfully to " << mall->name << "!" << endl;
    }
    
    // Search products by category
    void searchByCategory() {
        char category[50];
        cout << "\n=== Search Products by Category ===" << endl;
        cout << "Enter category: ";
        cin.ignore();
        cin.getline(category, 50);
        toUpperCase(category);
        
        LinkedList* catList = (LinkedList*)categoryIndex->search(category);
        if (catList == nullptr || catList->isEmpty()) {
            cout << "[INFO] No products found in this category." << endl;
            return;
        }
        
        cout << "\n--- Products in Category: " << category << " ---" << endl;
        Node* current = catList->getHead();
        while (current != nullptr) {
            Product* prod = (Product*)productTable->search(current->data);
            if (prod != nullptr) {
                cout << "  - " << prod->name << endl;
                cout << "    Price: $" << prod->price << endl;
            }
            current = current->next;
        }
    }
    
    // finding nearest mall
    void findNearestMall() {
        char userSector[50];
        cout << "\n=== Find Nearest Mall ===" << endl;
        cout << "Enter your sector (e.g., G-10): ";
        cin >> userSector;
        normalizeInput(userSector);
        
        char** mallKeys = new char*[100];
        int count;
        mallTable->getAllKeys(mallKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No malls registered yet." << endl;
            delete[] mallKeys;
            return;
        }
        
        bool found = false;
        cout << "\n--- Malls in Your Sector ---" << endl;
        for (int i = 0; i < count; i++) {
            Mall* mall = (Mall*)mallTable->search(mallKeys[i]);
            if (mall != nullptr) {
                char mallSector[50];
                stringCopy(mallSector, mall->sector);
                toUpperCase(mallSector);
                
                if (stringCompare(mallSector, userSector) == 0) {
                    cout << "  - " << mall->name << endl;
                    found = true;
                }
            }
        }
        
        delete[] mallKeys;
        
        if (!found) {
            cout << "[INFO] No malls found in your sector." << endl;
        }
    }
    
    // display mall
    void displayMall(const char* mallID) {
        Mall* mall = (Mall*)mallTable->search(mallID);
        if (mall == nullptr) {
            cout << "Mall not found!" << endl;
            return;
        }
        
        cout << "\n--- Mall Details ---" << endl;
        cout << "Mall ID: " << mall->mallID << endl;
        cout << "Name: " << mall->name << endl;
        cout << "Sector: " << mall->sector << endl;
    }
    
    // Get stats
    int getMallCount() { return mallCount; }
    int getProductCount() { return productCount; }
    const char* getLastRegisteredSector() const { return lastRegisteredSector; }
    
    // Display all summary
    void displayAllSummary() {
        cout << "\n========== COMMERCIAL SECTOR SUMMARY ==========" << endl;
        cout << "Total Malls: " << mallCount << endl;
        cout << "Total Products: " << productCount << endl;
        cout << "=============================================" << endl;
    }
    
    // Display all malls
    void displayAllMalls() {
        cout << "\n========== ALL SHOPPING MALLS ==========" << endl;
        
        char** mallKeys = new char*[100];
        int count;
        mallTable->getAllKeys(mallKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No malls registered yet." << endl;
            delete[] mallKeys;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            Mall* mall = (Mall*)mallTable->search(mallKeys[i]);
            if (mall != nullptr) {
                cout << "\n[" << (i+1) << "] " << mall->name << endl;
                cout << "    ID: " << mall->mallID << endl;
                cout << "    Sector: " << mall->sector << endl;
            }
        }
        
        delete[] mallKeys;
        cout << "\n========================================" << endl;
    }
    
    // Display all products
    void displayAllProducts() {
        cout << "\n========== ALL PRODUCTS ==========" << endl;
        
        char** prodKeys = new char*[500];
        int count;
        productTable->getAllKeys(prodKeys, &count);
        
        if (count == 0) {
            cout << "[INFO] No products registered yet." << endl;
            delete[] prodKeys;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            Product* prod = (Product*)productTable->search(prodKeys[i]);
            if (prod != nullptr) {
                cout << "\n[" << (i+1) << "] " << prod->name << endl;
                cout << "    ID: " << prod->productID << endl;
                cout << "    Category: " << prod->category << endl;
                cout << "    Price: $" << prod->price << endl;
            }
        }
        
        delete[] prodKeys;
        cout << "\n==================================" << endl;
    }
    
    // Mall visualization helpers
    void getMallKeys(char** keys, int* count) {
        mallTable->getAllKeys(keys, count);
    }
    
    Mall* getMall(const char* mallID) {
        return (Mall*)mallTable->search(mallID);
    }
    
    ~CommercialSystem() {
        delete mallTable;
        delete productTable;
        delete categoryIndex;
    }
};

#endif
