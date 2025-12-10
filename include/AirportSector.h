#ifndef AIRPORTSECTOR_H
#define AIRPORTSECTOR_H

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

// Airport structure
struct Airport {
    char code[10];
    char name[100];
    char sector[50];
    int terminals;
    
    Airport() {
        code[0] = '\0';
        name[0] = '\0';
        sector[0] = '\0';
        terminals = 1;
    }
};

// Flight structure (keeping it simple)
struct Flight {
    char flightNo[20];
    char airline[100];
    char departureAirport[10];
    char arrivalAirport[10];
    char currentLocation[10];
    char departureTime[20];
    char arrivalTime[20];
    
    Flight() {
        flightNo[0] = '\0';
        airline[0] = '\0';
        departureAirport[0] = '\0';
        arrivalAirport[0] = '\0';
        currentLocation[0] = '\0';
        departureTime[0] = '\0';
        arrivalTime[0] = '\0';
    }
};

// Airport system class
class AirportSystem {
private:
    Graph* airportGraph;
    HashTable* airportTable;
    HashTable* flightTable;
    
    int airportCount;
    int flightCount;
    int routeEdgeCount;

public:
    AirportSystem() {
        airportGraph = new Graph(50);
        airportTable = new HashTable(30);
        flightTable = new HashTable(100);
        airportCount = 0;
        flightCount = 0;
        routeEdgeCount = 0;
    }
    
    // 1. Register airport
    void registerAirport(const char* code, const char* name, const char* sector, int terminals) {
        Airport* airport = new Airport();
        stringCopyN(airport->code, code, 10);
        stringCopyN(airport->name, name, 100);
        stringCopyN(airport->sector, sector, 50);
        airport->terminals = terminals;
        
        airportTable->insert(code, airport);
        airportGraph->addVertex(code, 0, 0);
        airportCount++;
        
        // all good, registered quietly
    }
    
    void registerAirportManual() {
        char code[10], name[100], sector[50];
        int terminals;
        
        cout << "\n=== Register New Airport ===" << endl;
        cout << "Enter airport code (e.g., ISB, LHE): ";
        cin >> code;
        normalizeInput(code);
        
        cin.ignore();
        cout << "Enter airport name: ";
        cin.getline(name, 100);
        
        cout << "Enter sector: ";
        cin >> sector;
        populationSystem->autoRegisterSector(sector);
        
        cout << "Enter number of terminals: ";
        cin >> terminals;
        
        registerAirport(code, name, sector, terminals);
        cout << "\n[OK] Airport registered successfully!" << endl;
    }
    
    // 2. Display all airports
    void displayAllAirports() {
        cout << "\n========== ALL AIRPORTS ==========" << endl;
        
        char** keys = new char*[100];
        int count;
        airportTable->getAllKeys(keys, &count);
        
        if (count == 0) {
            cout << "[INFO] No airports registered yet." << endl;
            delete[] keys;
            return;
        }
        
        for (int i = 0; i < count; i++) {
            Airport* airport = (Airport*)airportTable->search(keys[i]);
            if (airport != nullptr) {
                cout << airport->code << " - " << airport->name << " [" << airport->sector << "]" << endl;
            }
        }
        
        delete[] keys;
        cout << "===================================" << endl;
    }
    
    // 3. Add flight
    void addFlight(const char* flightNo, const char* airline, const char* departure, 
                   const char* arrival, const char* depTime, const char* arrTime) {
        Flight* flight = new Flight();
        stringCopyN(flight->flightNo, flightNo, 20);
        stringCopyN(flight->airline, airline, 100);
        stringCopyN(flight->departureAirport, departure, 10);
        stringCopyN(flight->arrivalAirport, arrival, 10);
        stringCopyN(flight->currentLocation, departure, 10);
        stringCopyN(flight->departureTime, depTime, 20);
        stringCopyN(flight->arrivalTime, arrTime, 20);
        
        flightTable->insert(flightNo, flight);
        flightCount++;
        
        cout << "Added flight: " << flightNo << " (" << airline << ")" << endl;
    }
    
    void addFlightManual() {
        char flightNo[20], airline[100], departure[10], arrival[10];
        char depTime[20], arrTime[20];
        
        cout << "\n=== Add Flight ===" << endl;
        cout << "Enter flight number: ";
        cin >> flightNo;
        
        cin.ignore();
        cout << "Enter airline: ";
        cin.getline(airline, 100);
        
        cout << "Enter departure airport code: ";
        cin >> departure;
        normalizeInput(departure);
        
        cout << "Enter arrival airport code: ";
        cin >> arrival;
        normalizeInput(arrival);
        
        if (!airportTable->contains(departure)) {
            cout << "\n[ERROR] Departure airport " << departure << " does not exist!" << endl;
            return;
        }
        if (!airportTable->contains(arrival)) {
            cout << "\n[ERROR] Arrival airport " << arrival << " does not exist!" << endl;
            return;
        }
        
        cout << "Enter departure time (HH:MM): ";
        cin >> depTime;
        
        cout << "Enter arrival time (HH:MM): ";
        cin >> arrTime;
        
        addFlight(flightNo, airline, departure, arrival, depTime, arrTime);
        cout << "\n[OK] Flight added successfully!" << endl;
    }
    
    // 4. Display all flights
    void displayAllFlights() {
        cout << "\n========== ALL FLIGHTS ==========" << endl;
        
        char** keys = new char*[200];
        int count;
        flightTable->getAllKeys(keys, &count);
        
        if (count == 0) {
            cout << "[INFO] No flights registered yet." << endl;
            delete[] keys;
            return;
        }
        
        cout << "Total Flights: " << count << endl << endl;
        for (int i = 0; i < count; i++) {
            Flight* flight = (Flight*)flightTable->search(keys[i]);
            if (flight != nullptr) {
                cout << flight->flightNo << " - " << flight->airline << endl;
                cout << "  " << flight->departureAirport << " → " << flight->arrivalAirport;
                cout << " (" << flight->departureTime << " - " << flight->arrivalTime << ")" << endl;
            }
        }
        
        delete[] keys;
        cout << "=================================" << endl;
    }
    
    // 5. Get specific flight details
    void getFlightDetails() {
        char flightNo[20];
        cout << "\n=== Get Flight Details ===" << endl;
        cout << "Enter flight number: ";
        cin >> flightNo;
        normalizeInput(flightNo);
        
        Flight* flight = (Flight*)flightTable->search(flightNo);
        if (flight == nullptr) {
            cout << "\n[ERROR] Flight " << flightNo << " not found!" << endl;
            return;
        }
        
        cout << "\n--- Flight: " << flight->flightNo << " ---" << endl;
        cout << "Airline: " << flight->airline << endl;
        cout << "Route: " << flight->departureAirport << " → " << flight->arrivalAirport << endl;
        cout << "Schedule: " << flight->departureTime << " - " << flight->arrivalTime << endl;
        cout << "Current Location: " << flight->currentLocation << endl;
    }
    
    // 6. Update flight location
    void updateFlightLocation() {
        char flightNo[20], newLocation[10];
        
        cout << "\n=== Update Flight Location ===" << endl;
        cout << "Enter flight number: ";
        cin >> flightNo;
        normalizeInput(flightNo);
        
        Flight* flight = (Flight*)flightTable->search(flightNo);
        if (flight == nullptr) {
            cout << "\n[ERROR] Flight not found!" << endl;
            return;
        }
        
        cout << "Current location: " << flight->currentLocation << endl;
        cout << "Enter new airport code: ";
        cin >> newLocation;
        normalizeInput(newLocation);
        
        if (!airportTable->contains(newLocation)) {
            cout << "\n[ERROR] Airport " << newLocation << " does not exist!" << endl;
            return;
        }
        
        stringCopyN(flight->currentLocation, newLocation, 10);
        cout << "\n[OK] Flight location updated to: " << newLocation << endl;
    }
    
    // 7. Track specific flight
    void trackFlightLocation() {
        char flightNo[20];
        
        cout << "\n=== Track Flight Location ===" << endl;
        cout << "Enter flight number: ";
        cin >> flightNo;
        normalizeInput(flightNo);
        
        Flight* flight = (Flight*)flightTable->search(flightNo);
        if (flight == nullptr) {
            cout << "\n[ERROR] Flight not found!" << endl;
            return;
        }
        
        cout << "\n--- Flight " << flight->flightNo << " Location ---" << endl;
        cout << "Airline: " << flight->airline << endl;
        cout << "Current Location: " << flight->currentLocation << endl;
        cout << "Destination: " << flight->arrivalAirport << endl;
    }
    
    // 8. Find nearest airport
    void findNearestAirport() {
        char userSector[50];
        
        cout << "\n=== Find Nearest Airport ===" << endl;
        cout << "Enter your sector: ";
        cin >> userSector;
        normalizeInput(userSector);
        
        char** keys = new char*[100];
        int count;
        airportTable->getAllKeys(keys, &count);
        
        bool found = false;
        cout << "\n--- Airports in Your Sector ---" << endl;
        
        for (int i = 0; i < count; i++) {
            Airport* airport = (Airport*)airportTable->search(keys[i]);
            if (airport != nullptr) {
                char airportSector[50];
                stringCopy(airportSector, airport->sector);
                normalizeInput(airportSector);
                
                if (stringCompare(airportSector, userSector) == 0) {
                    cout << "  - " << airport->name << " (" << airport->code << ")" << endl;
                    found = true;
                }
            }
        }
        
        delete[] keys;
        
        if (!found) {
            cout << "[INFO] No airports found in your sector." << endl;
        }
    }
    
    // 9. Connect airports manually
    void connectAirportsManual() {
        char airport1[10], airport2[10];
        double distance;
        
        cout << "\n=== Connect Two Airports ===" << endl;
        cout << "Enter first airport code: ";
        cin >> airport1;
        normalizeInput(airport1);
        
        cout << "Enter second airport code: ";
        cin >> airport2;
        normalizeInput(airport2);
        
        if (!airportTable->contains(airport1) || !airportTable->contains(airport2)) {
            cout << "\n[ERROR] One or both airports not found!" << endl;
            return;
        }
        
        cout << "Enter distance (km): ";
        cin >> distance;
        
        airportGraph->addBidirectionalEdge(airport1, airport2, distance);
        routeEdgeCount++;
        cout << "\n[OK] Airports connected successfully!" << endl;
    }
    
    // 10. Find shortest path
    void findShortestPath() {
        char from[10], to[10];
        
        cout << "\n=== Find Shortest Path Between Airports ===" << endl;
        cout << "Enter source airport code: ";
        cin >> from;
        normalizeInput(from);
        
        cout << "Enter destination airport code: ";
        cin >> to;
        normalizeInput(to);
        
        if (!airportTable->contains(from) || !airportTable->contains(to)) {
            cout << "\n[ERROR] One or both airports not found!" << endl;
            return;
        }
        
        int vertexCount = airportGraph->getVertexCount();
        double* distances = new double[vertexCount];
        int* previous = new int[vertexCount];
        
        airportGraph->dijkstra(from, distances, previous);
        
        int destIndex = -1;
        for (int i = 0; i < vertexCount; i++) {
            char vertexName[50];
            stringCopy(vertexName, airportGraph->getVertexName(i));
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
            cout << "\n[ERROR] No path exists between these airports!" << endl;
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
            stringCopy(path[pathSize], airportGraph->getVertexName(current));
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
        cout << "\n========== AIRPORT NETWORK SUMMARY ==========" << endl;
        cout << "Total Airports: " << airportCount << endl;
        cout << "Total Connections: " << routeEdgeCount << endl;
        cout << "Total Flights: " << flightCount << endl << endl;
        
        cout << "Airport connections:" << endl;
        airportGraph->display();
        cout << "=============================================" << endl;
    }
    
    // 12. Simulate passenger boarding queue
    void simulatePassengerBoarding() {
        char airportCode[10];
        cout << "\n=== Passenger Boarding Simulation (Circular Queue) ===" << endl;
        cout << "Enter airport code: ";
        cin >> airportCode;
        normalizeInput(airportCode);
        
        if (!airportTable->contains(airportCode)) {
            cout << "[ERROR] Airport not found!" << endl;
            return;
        }
        
        Airport* airport = (Airport*)airportTable->search(airportCode);
        cout << "Airport: " << airport->name << " (" << airportCode << ")" << endl;
        
        Queue passengerQueue(15);
        
        cout << "\nPassengers joining queue..." << endl;
        for (int i = 1; i <= 8; i++) {
            passengerQueue.enqueue(i);
            cout << "  Passenger " << i << " joined queue" << endl;
        }
        
        cout << "\nBoarding passengers onto flights..." << endl;
        while (!passengerQueue.isEmpty()) {
            int passenger = passengerQueue.dequeue();
            cout << "  Passenger " << passenger << " boarded" << endl;
        }
        
        cout << "\n[OK] All passengers boarded successfully!" << endl;
    }
    
    // grabbing stats
    int getAirportCount() { return airportCount; }
    int getFlightCount() { return flightCount; }
    int getRouteEdgeCount() { return routeEdgeCount; }
    
    // Airport visualization helpers
    void getAirportKeys(char** keys, int* count) {
        airportTable->getAllKeys(keys, count);
    }
    
    Airport* getAirport(const char* airportID) {
        return (Airport*)airportTable->search(airportID);
    }
    
    // Flight visualization helpers
    void getFlightKeys(char** keys, int* count) {
        flightTable->getAllKeys(keys, count);
    }
    
    Flight* getFlight(const char* flightNo) {
        return (Flight*)flightTable->search(flightNo);
    }
    
    // Get distance between two airports
    double getDistance(const char* airport1, const char* airport2) {
        return airportGraph->getEdgeWeight(airport1, airport2);
    }
    
    // Get graph for pathfinding visualization
    Graph* getAirportGraph() {
        return airportGraph;
    }
    
    // Get visual path between two airports (for visualization)
    int getVisualPath(const char* from, const char* to, char pathIds[][50], double& distance) {
        int vertexCount = airportGraph->getVertexCount();
        double* distances = new double[vertexCount];
        int* previous = new int[vertexCount];
        
        airportGraph->dijkstra(from, distances, previous);
        
        // Find destination index
        int destIndex = -1;
        for (int i = 0; i < vertexCount; i++) {
            if (strcmp(airportGraph->getVertexName(i), to) == 0) {
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
        airportGraph->getPath(previous, destIndex, path, pathLength);
        
        for (int i = 0; i < pathLength && i < 50; i++) {
            strcpy(pathIds[i], airportGraph->getVertexName(path[i]));
        }
        
        distance = distances[destIndex];
        
        delete[] distances;
        delete[] previous;
        return pathLength;
    }
    
    ~AirportSystem() {
        delete airportGraph;
        delete airportTable;
        delete flightTable;
    }
};

#endif
