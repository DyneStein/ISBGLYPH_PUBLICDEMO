#ifndef RAILWAYSECTOR_H
#define RAILWAYSECTOR_H

#include "Graph.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "Queue.h"
#include "Utils.h"
#include <iostream>
using namespace std;

// Forward declaration
class PopulationSystem;
extern PopulationSystem* populationSystem;

// Railway Station structure
struct RailStation {
    char stationID[20];
    char name[100];
    char sector[50];
    int platforms;
    
    RailStation() {
        stationID[0] = '\0';
        name[0] = '\0';
        sector[0] = '\0';
        platforms = 1;
    }
};

// Train structure (keeping it simple)
struct Train {
    char trainNo[20];
    char name[100];
    char departureStation[20];
    char arrivalStation[20];
    char currentLocation[20];
    char departureTime[20];
    char arrivalTime[20];
    
    Train() {
        trainNo[0] = '\0';
        name[0] = '\0';
        departureStation[0] = '\0';
        arrivalStation[0] = '\0';
        currentLocation[0] = '\0';
        departureTime[0] = '\0';
        arrivalTime[0] = '\0';
    }
};

// Railway system class
class RailwaySystem {
private:
    Graph* stationGraph;
    HashTable* stationTable;
    HashTable* trainTable;
    
    int stationCount;
    int trainCount;
    int routeEdgeCount;

public:
    RailwaySystem() {
        stationGraph = new Graph(50);
        stationTable = new HashTable(30);
        trainTable = new HashTable(100);
        stationCount = 0;
        trainCount = 0;
        routeEdgeCount = 0;
    }
    
    // 1. Register station
    void registerStation(const char* stationID, const char* name, const char* sector, int platforms) {
        RailStation* station = new RailStation();
        stringCopyN(station->stationID, stationID, 20);
        stringCopyN(station->name, name, 100);
        stringCopyN(station->sector, sector, 50);
        station->platforms = platforms;
        
        stationTable->insert(stationID, station);
        stationGraph->addVertex(stationID, 0, 0);
        stationCount++;
        
        // all good, registered quietly
    }
    
    void registerStationManual() {
        char stationID[20], name[100], sector[50];
        int platforms;
        
        cout << "\n=== Register New Railway Station ===" << endl;
        
        generateUniqueID("STN", stationID);
        cout << "Generated Station ID: " << stationID << endl;
        
        cin.ignore();
        cout << "Enter station name: ";
        cin.getline(name, 100);
        
        cout << "Enter sector: ";
        cin >> sector;
        populationSystem->autoRegisterSector(sector);
        
        cout << "Enter number of platforms: ";
        cin >> platforms;
        
        registerStation(stationID, name, sector, platforms);
        cout << "\n[OK] Station registered successfully!" << endl;
    }
    
    // 2. Display all stations
    void displayAllStations() {
        cout << "\n========== ALL RAILWAY STATIONS ==========" << endl;
        
        char** keys = new char*[100];
        int count;
        stationTable->getAllKeys(keys, &count);
        
        if (count == 0) {
            cout << "[INFO] No stations registered yet." << endl;
            delete[] keys;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            RailStation* station = (RailStation*)stationTable->search(keys[i]);
            if (station != nullptr) {
                cout << station->stationID << " - " << station->name << " [" << station->sector << "]" << endl;
            }
        }
        
        delete[] keys;
        cout << "==========================================" << endl;
    }
    
    // 3. Add train
    void addTrain(const char* trainNo, const char* name, const char* departure, 
                  const char* arrival, const char* depTime, const char* arrTime) {
        Train* train = new Train();
        stringCopyN(train->trainNo, trainNo, 20);
        stringCopyN(train->name, name, 100);
        stringCopyN(train->departureStation, departure, 20);
        stringCopyN(train->arrivalStation, arrival, 20);
        stringCopyN(train->currentLocation, departure, 20);
        stringCopyN(train->departureTime, depTime, 20);
        stringCopyN(train->arrivalTime, arrTime, 20);
        
        trainTable->insert(trainNo, train);
        trainCount++;
        
        cout << "Added train: " << trainNo << " (" << name << ")" << endl;
    }
    
    void addTrainManual() {
        char trainNo[20], name[100], departure[20], arrival[20];
        char depTime[20], arrTime[20];
        
        cout << "\n=== Add Train ===" << endl;
        cout << "Enter train number: ";
        cin >> trainNo;
        
        cin.ignore();
        cout << "Enter train name: ";
        cin.getline(name, 100);
        
        cout << "Enter departure station ID: ";
        cin >> departure;
        normalizeInput(departure);
        
        cout << "Enter arrival station ID: ";
        cin >> arrival;
        normalizeInput(arrival);
        
        if (!stationTable->contains(departure)) {
            cout << "\n[ERROR] Departure station " << departure << " does not exist!" << endl;
            return;
        }
        if (!stationTable->contains(arrival)) {
            cout << "\n[ERROR] Arrival station " << arrival << " does not exist!" << endl;
            return;
        }
        
        cout << "Enter departure time (HH:MM): ";
        cin >> depTime;
        
        cout << "Enter arrival time (H:MM): ";
        cin >> arrTime;
        
        addTrain(trainNo, name, departure, arrival, depTime, arrTime);
        cout << "\n[OK] Train added successfully!" << endl;
    }
    
    // 4. Display all trains
    void displayAllTrains() {
        cout << "\n========== ALL TRAINS ==========" << endl;
        
        char** keys = new char*[200];
        int count;
        trainTable->getAllKeys(keys, &count);
        
        if (count == 0) {
            cout << "[INFO] No trains registered yet." << endl;
            delete[] keys;
            return;
        }
        
        cout << "Total Trains: " << count << endl << endl;
        for (int i = 0; i < count; i++) {
            Train* train = (Train*)trainTable->search(keys[i]);
            if (train != nullptr) {
                cout << train->trainNo << " - " << train->name << endl;
                cout << "  " << train->departureStation << " → " << train->arrivalStation;
                cout << " (" << train->departureTime << " - " << train->arrivalTime << ")" << endl;
            }
        }
        
        delete[] keys;
        cout << "================================" << endl;
    }
    
    // 5. Get specific train details
    void getTrainDetails() {
        char trainNo[20];
        cout << "\n=== Get Train Details ===" << endl;
        cout << "Enter train number: ";
        cin >> trainNo;
        normalizeInput(trainNo);
        
        Train* train = (Train*)trainTable->search(trainNo);
        if (train == nullptr) {
            cout << "\n[ERROR] Train " << trainNo << " not found!" << endl;
            return;
        }
        
        cout << "\n--- Train: " << train->trainNo << " ---" << endl;
        cout << "Name: " << train->name << endl;
        cout << "Route: " << train->departureStation << " → " << train->arrivalStation << endl;
        cout << "Schedule: " << train->departureTime << " - " << train->arrivalTime << endl;
        cout << "Current Location: " << train->currentLocation << endl;
    }
    
    // 6. Update train location
    void updateTrainLocation() {
        char trainNo[20], newLocation[20];
        
        cout << "\n=== Update Train Location ===" << endl;
        cout << "Enter train number: ";
        cin >> trainNo;
        normalizeInput(trainNo);
        
        Train* train = (Train*)trainTable->search(trainNo);
        if (train == nullptr) {
            cout << "\n[ERROR] Train not found!" << endl;
            return;
        }
        
        cout << "Current location: " << train->currentLocation << endl;
        cout << "Enter new station ID: ";
        cin >> newLocation;
        normalizeInput(newLocation);
        
        if (!stationTable->contains(newLocation)) {
            cout << "\n[ERROR] Station " << newLocation << " does not exist!" << endl;
            return;
        }
        
        stringCopyN(train->currentLocation, newLocation, 20);
        cout << "\n[OK] Train location updated to: " << newLocation << endl;
    }
    
    // 7. Track specific train
    void trackTrainLocation() {
        char trainNo[20];
        
        cout << "\n=== Track Train Location ===" << endl;
        cout << "Enter train number: ";
        cin >> trainNo;
        normalizeInput(trainNo);
        
        Train* train = (Train*)trainTable->search(trainNo);
        if (train == nullptr) {
            cout << "\n[ERROR] Train not found!" << endl;
            return;
        }
        
        cout << "\n--- Train " << train->trainNo << " Location ---" << endl;
        cout << "Name: " << train->name << endl;
        cout << "Current Location: " << train->currentLocation << endl;
        cout << "Destination: " << train->arrivalStation << endl;
    }
    
    // 8. Find nearest station
    void findNearestStation() {
        char userSector[50];
        
        cout << "\n=== Find Nearest Railway Station ===" << endl;
        cout << "Enter your sector: ";
        cin >> userSector;
        normalizeInput(userSector);
        
        char** keys = new char*[100];
        int count;
        stationTable->getAllKeys(keys, &count);
        
        bool found = false;
        cout << "\n--- Stations in Your Sector ---" << endl;
        
        for (int i = 0; i < count; i++) {
            RailStation* station = (RailStation*)stationTable->search(keys[i]);
            if (station != nullptr) {
                char stationSector[50];
                stringCopy(stationSector, station->sector);
                normalizeInput(stationSector);
                
                if (stringCompare(stationSector, userSector) == 0) {
                    cout << "  - " << station->name << " (" << station->stationID << ")" << endl;
                    found = true;
                }
            }
        }
        
        delete[] keys;
        
        if (!found) {
            cout << "[INFO] No railway stations found in your sector." << endl;
        }
    }
    
    // 9. Connect stations manually
    void connectStationsManual() {
        char station1[20], station2[20];
        double distance;
        
        cout << "\n=== Connect Two Stations ===" << endl;
        cout << "Enter first station ID: ";
        cin >> station1;
        normalizeInput(station1);
        
        cout << "Enter second station ID: ";
        cin >> station2;
        normalizeInput(station2);
        
        if (!stationTable->contains(station1) || !stationTable->contains(station2)) {
            cout << "\n[ERROR] One or both stations not found!" << endl;
            return;
        }
        
        cout << "Enter distance (km): ";
        cin >> distance;
        
        stationGraph->addBidirectionalEdge(station1, station2, distance);
        routeEdgeCount++;
        cout << "\n[OK] Stations connected successfully!" << endl;
    }
    
    // 10. Find shortest path
    void findShortestPath() {
        char from[20], to[20];
        
        cout << "\n=== Find Shortest Path Between Stations ===" << endl;
        cout << "Enter source station ID: ";
        cin >> from;
        normalizeInput(from);
        
        cout << "Enter destination station ID: ";
        cin >> to;
        normalizeInput(to);
        
        if (!stationTable->contains(from) || !stationTable->contains(to)) {
            cout << "\n[ERROR] One or both stations not found!" << endl;
            return;
        }
        
        int vertexCount = stationGraph->getVertexCount();
        double* distances = new double[vertexCount];
        int* previous = new int[vertexCount];
        
        stationGraph->dijkstra(from, distances, previous);
        
        int destIndex = -1;
        for (int i = 0; i < vertexCount; i++) {
            char vertexName[50];
            stringCopy(vertexName, stationGraph->getVertexName(i));
            toUpperCase(vertexName);
            
            char toUpper[50];
            stringCopy(toUpper, to);
            toUpperCase(toUpper);
            
            if (stringCompare(vertexName, toUpper) == 0) {
                destIndex = i;
                break;
            }
        }
        
        if (destIndex == -1 || distances[destIndex] >= GRAPH_MAX_DISTANCE) {
            cout << "\n[ERROR] No path exists between these stations!" << endl;
            delete[] distances;
            delete[] previous;
            return;
        }
        
        cout << "\nShortest distance: " << distances[destIndex] << " km" << endl;
        cout << "Path: ";
        
        int pathSize = 0;
        char** path = new char*[vertexCount];
        int current = destIndex;
        
        while (current != -1) {
            path[pathSize] = new char[50];
            stringCopy(path[pathSize], stationGraph->getVertexName(current));
            pathSize++;
            current = previous[current];
        }
        
        for (int i = pathSize - 1; i >= 0; i--) {
            cout << path[i];
            if (i > 0) cout << " → ";
            delete[] path[i];
        }
        cout << endl;
        
        delete[] path;
        delete[] distances;
        delete[] previous;
    }
    
    // 11. Display route network summary
    void displayRouteNetworkSummary() {
        cout << "\n========== RAILWAY NETWORK SUMMARY ==========" << endl;
        cout << "Total Stations: " << stationCount << endl;
        cout << "Total Connections: " << routeEdgeCount << endl;
        cout << "Total Trains: " << trainCount << endl << endl;
        
        cout << "Station connections:" << endl;
        stationGraph->display();
        cout << "=============================================" << endl;
    }
    
    // 12. Simulate passenger boarding queue
    void simulatePassengerBoarding() {
        char stationID[20];
        cout << "\n=== Passenger Boarding Simulation (Circular Queue) ===" << endl;
        cout << "Enter station ID: ";
        cin >> stationID;
        normalizeInput(stationID);
        
        if (!stationTable->contains(stationID)) {
            cout << "[ERROR] Station not found!" << endl;
            return;
        }
        
        RailStation* station = (RailStation*)stationTable->search(stationID);
        cout << "Station: " << station->name << " (" << stationID << ")" << endl;
        
        Queue passengerQueue(15);
        
        cout << "\nPassengers joining queue..." << endl;
        for (int i = 1; i <= 8; i++) {
            passengerQueue.enqueue(i);
            cout << "  Passenger " << i << " joined queue" << endl;
        }
        
        cout << "\nBoarding passengers onto trains..." << endl;
        while (!passengerQueue.isEmpty()) {
            int passenger = passengerQueue.dequeue();
            cout << "  Passenger " << passenger << " boarded" << endl;
        }
        
        cout << "\n[OK] All passengers boarded successfully!" << endl;
    }
    
    // grabbing stats
    int getStationCount() { return stationCount; }
    int getTrainCount() { return trainCount; }
    int getRouteEdgeCount() { return routeEdgeCount; }
    
    // Railway station visualization helpers
    void getStationKeys(char** keys, int* count) {
        stationTable->getAllKeys(keys, count);
    }
    
    RailStation* getStation(const char* stationID) {
        return (RailStation*)stationTable->search(stationID);
    }
    
    // Train visualization helpers
    void getTrainKeys(char** keys, int* count) {
        trainTable->getAllKeys(keys, count);
    }
    
    Train* getTrain(const char* trainNo) {
        return (Train*)trainTable->search(trainNo);
    }
    
    // Get distance between two railway stations
    double getDistance(const char* station1, const char* station2) {
        return stationGraph->getEdgeWeight(station1, station2);
    }
    
    // Get graph for pathfinding visualization
    Graph* getStationGraph() {
        return stationGraph;
    }
    
    // Get visual path between two stations (for visualization)
    int getVisualPath(const char* from, const char* to, char pathIds[][50], double& distance) {
        int vertexCount = stationGraph->getVertexCount();
        double* distances = new double[vertexCount];
        int* previous = new int[vertexCount];
        
        stationGraph->dijkstra(from, distances, previous);
        
        // Find destination index
        int destIndex = -1;
        for (int i = 0; i < vertexCount; i++) {
            if (strcmp(stationGraph->getVertexName(i), to) == 0) {
                destIndex = i;
                break;
            }
        }
        
        if (destIndex == -1 || distances[destIndex] >= 999999) {
            delete[] distances;
            delete[] previous;
            distance = 0;
            return 0;
        }
        
        int path[100];
        int pathLength = 0;
        stationGraph->getPath(previous, destIndex, path, pathLength);
        
        for (int i = 0; i < pathLength && i < 50; i++) {
            strcpy(pathIds[i], stationGraph->getVertexName(path[i]));
        }
        
        distance = distances[destIndex];
        
        delete[] distances;
        delete[] previous;
        return pathLength;
    }
    
    ~RailwaySystem() {
        delete stationGraph;
        delete stationTable;
        delete trainTable;
    }
};

#endif
