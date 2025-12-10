#ifndef TRANSPORTSECTOR_H
#define TRANSPORTSECTOR_H

#include "Graph.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "Stack.h"
#include "MinHeap.h"
#include "Utils.h"
#include "Queue.h"
#include <iostream>
using namespace std;

// bus stop structure
struct BusStop {
    char stopID[50];
    char name[100];
    char sector[50];  // added sector field here
    double latitude;
    double longitude;
    
    BusStop() {
        stopID[0] = '\0';
        name[0] = '\0';
        sector[0] = '\0';
        latitude = 0;
        longitude = 0;
    }
};

// bus structure
struct Bus {
    char busNo[20];
    char company[100];
    char currentStop[50];
    LinkedList* route; // keeping track of stop IDs
    
    Bus() {
        busNo[0] = '\0';
        company[0] = '\0';
        currentStop[0] = '\0';
        route = new LinkedList();
    }
    
    ~Bus() {
        delete route;
    }
};

// bonus feature: school bus structure
struct SchoolBus {
    char busID[20];
    char schoolID[50];
    LinkedList* studentStops;   // stops where students board
    Stack* visitedStops;        // history of visited stops
    char currentLocation[50];
    int studentsOnBoard;
    int maxCapacity;
    
    SchoolBus() {
        busID[0] = '\0';
        schoolID[0] = '\0';
        studentStops = new LinkedList();
        visitedStops = new Stack(50);
        currentLocation[0] = '\0';
        studentsOnBoard = 0;
        maxCapacity = 50;
    }
    
    ~SchoolBus() {
        delete studentStops;
        delete visitedStops;
    }
};

// bonus feature: emergency vehicle structure
struct EmergencyVehicle {
    char vehicleID[20];
    char type[50];              // Ambulance, Fire Truck, Police
    char currentLocation[50];
    char destination[50];
    int priority;               // 1-10, 10 is highest
    bool active;
    
    EmergencyVehicle() {
        vehicleID[0] = '\0';
        type[0] = '\0';
        currentLocation[0] = '\0';
        destination[0] = '\0';
        priority = 5;
        active = false;
    }
};

// transport system class
class TransportSystem {
private:
    Graph* stopGraph;
    HashTable* busTable;
    HashTable* stopTable;
    Stack* routeHistory;
    
    // keeping track of the numbers
    int busCount;
    int stopCount;
    int routeEdgeCount;

public:
    TransportSystem() {
        stopGraph = new Graph(100);
        busTable = new HashTable(50);
        stopTable = new HashTable(50);
        routeHistory = new Stack(100);
        busCount = 0;
        stopCount = 0;
        routeEdgeCount = 0;
    }
    
    // adding bus stop
    void addBusStop(const char* stopID, const char* name, const char* sector, double lat, double lon) {
        BusStop* stop = new BusStop();
        stringCopyN(stop->stopID, stopID, 50);
        stringCopyN(stop->name, name, 100);
        stringCopyN(stop->sector, sector, 50);
        stop->latitude = lat;
        stop->longitude = lon;
        
        stopTable->insert(stopID, stop);
        stopGraph->addVertex(stopID, lat, lon);
        stopCount++;
        
        // Stop added (quiet mode)
    }
    
    // connecting two stops with distance
    void connectStops(const char* stop1, const char* stop2, double distance) {
        stopGraph->addBidirectionalEdge(stop1, stop2, distance);
        routeEdgeCount++;
    }
    
    // Connect two stops manually with user input
    void connectStopsManual() {
        char stop1[50], stop2[50];
        double distance;
        
        cout << "\n=== Connect Two Bus Stops ===" << endl;
        cout << "This creates a bidirectional route between two stops." << endl;
        cout << "\nEnter first stop ID: ";
        cin >> stop1;
        normalizeInput(stop1);
        
        cout << "Enter second stop ID: ";
        cin >> stop2;
        normalizeInput(stop2);
        
        // making sure Check if both stops exist
        BusStop* stopA = (BusStop*)stopTable->search(stop1);
        BusStop* stopB = (BusStop*)stopTable->search(stop2);
        
        if (stopA == nullptr) {
            cout << "\n[ERROR] Stop " << stop1 << " does not exist!" << endl;
            return;
        }
        if (stopB == nullptr) {
            cout << "\n[ERROR] Stop " << stop2 << " does not exist!" << endl;
            return;
        }
        
        cout << "Enter distance (km): ";
        cin >> distance;
        
        if (cin.fail() || distance <= 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[ERROR] Invalid distance!" << endl;
            return;
        }
        
        connectStops(stop1, stop2, distance);
        cout << "\n[OK] Connected " << stopA->name << " <-> " << stopB->name 
             << " (" << distance << " km)" << endl;
    }
    
    // register bus
    void registerBus(const char* busNo, const char* company, const char* currentStop) {
        Bus* bus = new Bus();
        stringCopyN(bus->busNo, busNo, 20);
        stringCopyN(bus->company, company, 100);
        stringCopyN(bus->currentStop, currentStop, 50);
        
        busTable->insert(busNo, bus);
        busCount++;
        
        // Bus added (quiet mode)
    }
    
    // add stop to bus route
    void addStopToRoute(const char* busNo, const char* stopID) {
        Bus* bus = (Bus*)busTable->search(busNo);
        if (bus == nullptr) {
            cout << "Bus not found!" << endl;
            return;
        }
        
        bus->route->append(stopID);
    }
    
    // get bus by number
    void getBusDetails(const char* busNo) {
        Bus* bus = (Bus*)busTable->search(busNo);
        if (bus == nullptr) {
            cout << "Bus not found!" << endl;
            return;
        }
        
        cout << "\n--- Bus Details ---" << endl;
        cout << "Bus Number: " << bus->busNo << endl;
        cout << "Company: " << bus->company << endl;
        cout << "Current Stop: " << bus->currentStop << endl;
        cout << "Route: ";
        
        Node* temp = bus->route->getHead();
        while (temp != nullptr) {
            cout << temp->data;
            if (temp->next != nullptr) cout << " > ";
            temp = temp->next;
        }
        cout << endl;
    }
    
    // find shortest path between two stops
    void findShortestPath(const char* from, const char* to) {
        int vertexCount = stopGraph->getVertexCount();
        double* distances = new double[vertexCount];
        int* previous = new int[vertexCount];
        
        stopGraph->dijkstra(from, distances, previous);
        
        // find destination index - CASE INSENSITIVE comparison
        int destIndex = -1;
        for (int i = 0; i < vertexCount; i++) {
            char vertexName[50];
            stringCopy(vertexName, stopGraph->getVertexName(i));
            toUpperCase(vertexName);  // Normalize to uppercase for comparison
            
            char toUpper[50];
            stringCopy(toUpper, to);
            toUpperCase(toUpper);
            
            if (stringCompare(vertexName, toUpper) == 0) {
                destIndex = i;
                break;
            }
        }
        
        if (destIndex == -1 || distances[destIndex] >= GRAPH_MAX_DISTANCE) {
            cout << "\n[ERROR] No path exists between these stops!" << endl;
            cout << "[TIP] The stops might not be connected. Use 'Display Route Network' to see connections." << endl;
            delete[] distances;
            delete[] previous;
            return;
        }
        
        // get path
        int path[100];
        int pathLength = 0;
        stopGraph->getPath(previous, destIndex, path, pathLength);
        
        // making sure Check if path is valid
        if (pathLength == 0) {
            cout << "\n[ERROR] No path found between these stops!" << endl;
            cout << "[TIP] Make sure both stops are connected to the route network." << endl;
            delete[] distances;
            delete[] previous;
            return;
        }
        
        cout << "\n--- Shortest Path ---" << endl;
        cout << "From: " << from << " To: " << to << endl;
        cout << "Distance: " << distances[destIndex] << " km" << endl;
        cout << "Path: ";
        
        for (int i = 0; i < pathLength; i++) {
            cout << stopGraph->getVertexName(path[i]);
            if (i < pathLength - 1) cout << " -> ";
        }
        cout << endl;
        
        delete[] distances;
        delete[] previous;
    }
    
    // update bus location
    void updateBusLocation(const char* busNo, const char* newStop) {
        Bus* bus = (Bus*)busTable->search(busNo);
        if (bus == nullptr) {
            cout << "Bus not found!" << endl;
            return;
        }
        
        // save to history
        char historyEntry[100];
        stringCopy(historyEntry, busNo);
        stringConcat(historyEntry, " at ");
        stringConcat(historyEntry, bus->currentStop);
        routeHistory->push(historyEntry);
        
        // update current stop
        stringCopyN(bus->currentStop, newStop, 50);
        cout << "Updated bus " << busNo << " location to " << newStop << endl;
    }
    
    
    // ========== MANUAL REGISTRATION METHODS ==========
    
    // Add bus stop manually with user input
    void addBusStopManual() {
        char stopID[50], name[100], sector[50];
        double lat, lon;
        
        cout << "\n=== Register New Bus Stop ===" << endl;
        
        // Generate unique ID
        generateUniqueID("STOP", stopID);
        cout << "Generated Stop ID: " << stopID << endl;
        
        cout << "Enter stop name: ";
        cin.ignore();
        cin.getline(name, 100);
        
        cout << "Enter sector (e.g., G-10, F-8): ";
        cin >> sector;
        
        // auto-registering sector to tree (cross-sector integration)
        populationSystem->autoRegisterSector(sector);
        
        cout << "Enter latitude: ";
        cin >> lat;
        
        cout << "Enter longitude: ";
        cin >> lon;
        
        addBusStop(stopID, name, sector, lat, lon);  // Now passes sector
        cout << "\n[OK] Bus stop registered successfully!" << endl;
    }
    
    // Register bus manually with route
    void registerBusManual() {
        char busNo[20], company[100], currentStop[50];
        int routeStopCount = -1;
        
        cout << "\n=== Register New Bus ===" << endl;
        
        cout << "Enter bus number (e.g., B101): ";
        cin >> busNo;
        normalizeInput(busNo);
        
        cout << "Enter company name: ";
        cin.ignore();
        cin.getline(company, 100);
        
        cout << "Enter current stop ID: ";
        cin >> currentStop;
        normalizeInput(currentStop);
        
        // making sure Check if stop exists
        BusStop* stop = (BusStop*)stopTable->search(currentStop);
        if (stop == nullptr) {
            cout << "\n[ERROR] Stop " << currentStop << " does not exist!" << endl;
            cout << "[TIP] Register the stop first using option [4], or check available stops." << endl;
            return;
        }
        
        // Register bus
        registerBus(busNo, company, currentStop);
        
        // Add route stops with INPUT VALIDATION
        cout << "\nHow many stops in the route? ";
        cin >> routeStopCount;
        
        // making sure Check if input is valid number
        if (cin.fail() || routeStopCount < 0) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[ERROR] Invalid number! Bus registered but route is empty." << endl;
            cout << "[TIP] You can add route stops later if needed." << endl;
            return;
        }
        
        if (routeStopCount == 0) {
            cout << "[INFO] No route stops added. Bus registered with empty route." << endl;
            return;
        }
        
        cout << "Enter stop IDs in order (press Enter after each):" << endl;
        for (int i = 0; i < routeStopCount; i++) {
            char stopID[50];
            cout << "Stop " << (i + 1) << ": ";
            cin >> stopID;
            normalizeInput(stopID);
            
            // making sure Check if each route stop exists
            BusStop* routeStop = (BusStop*)stopTable->search(stopID);
            if (routeStop == nullptr) {
                cout << "[WARNING] Stop " << stopID << " not found - skipping!" << endl;
                continue;
            }
            
            addStopToRoute(busNo, stopID);
        }
        
        cout << "\n[OK] Bus and route registered successfully!" << endl;
    }
    
    // Track specific bus location
    void trackBusLocation() {
        char busNo[20];
        cout << "\n=== Track Bus Location ===" << endl;
        cout << "Enter bus number: ";
        cin >> busNo;
        normalizeInput(busNo);
        
        Bus* bus = (Bus*)busTable->search(busNo);
        if (bus == nullptr) {
            cout << "[ERROR] Bus not found!" << endl;
            return;
        }
        
        cout << "\n--- Bus Location Info ---" << endl;
        cout << "Bus: " << bus->busNo << " (" << bus->company << ")" << endl;
        cout << "Current Stop: " << bus->currentStop << endl;
        
        // Show route
        if (bus->route != nullptr && bus->route->getSize() > 0) {
            cout << "Full Route: ";
            bus->route->display();
            
            // Predict next stop - CASE INSENSITIVE
            Node* current = bus->route->getHead();
            while (current != nullptr) {
                char routeStop[50], currentStop[50];
                stringCopy(routeStop, current->data);
                stringCopy(currentStop, bus->currentStop);
                toUpperCase(routeStop);
                toUpperCase(currentStop);
                
                if (stringCompare(routeStop, currentStop) == 0) {
                    if (current->next != nullptr) {
                        cout << "Next Stop: " << current->next->data << endl;
                    } else {
                        cout << "Next Stop: End of route" << endl;
                    }
                    break;
                }
                current = current->next;
            }
        }
    }
    
    // finding nearest bus to user location
    void findNearestBus() {
        double userLat, userLon;
        cout << "\n=== Find Nearest Bus ===" << endl;
        cout << "Enter your latitude: ";
        cin >> userLat;
        cout << "Enter your longitude: ";
        cin >> userLon;
        
        // Get all buses
        char** busKeys = new char*[100];
        int busCount;
        busTable->getAllKeys(busKeys, &busCount);
        
        if (busCount == 0) {
            cout << "[INFO] No buses registered yet." << endl;
            delete[] busKeys;
            return;
        }
        
        double minDistance = GRAPH_MAX_DISTANCE;
        char nearestBus[20] = "";
        char nearestStop[100] = "";
        
        // finding nearest
        for (int i = 0; i < busCount; i++) {
            Bus* bus = (Bus*)busTable->search(busKeys[i]);
            if (bus != nullptr) {
                BusStop* stop = (BusStop*)stopTable->search(bus->currentStop);
                
                // making sure Skip if stop doesn't exist
                if (stop == nullptr) {
                    continue;  // Bus has invalid location
                }
                
                double dist = calculateDistance(userLat, userLon, 
                                               stop->latitude, stop->longitude);
                if (dist < minDistance) {
                    minDistance = dist;
                    stringCopy(nearestBus, bus->busNo);
                    stringCopy(nearestStop, stop->name);
                }
            }
        }
        
        delete[] busKeys;
        
        if (nearestBus[0] != '\0') {
            cout << "\n--- Nearest Bus Found ---" << endl;
            cout << "Bus: " << nearestBus << endl;
            cout << "Currently at: " << nearestStop << endl;
            cout << "Distance: " << minDistance << " km" << endl;
        } else {
            cout << "[INFO] No bus locations available." << endl;
        }
    }
    
    // display all stops - NOW SHOWS ID, NAME, AND SECTOR
    void displayAllStops() {
        cout << "\n--- All Bus Stops ---" << endl;
        
        // Manually iterate through vertices
        int vertexCount = stopGraph->getVertexCount();
        for (int i = 0; i < vertexCount; i++) {
            const char* stopID = stopGraph->getVertexName(i);
            BusStop* stop = (BusStop*)stopTable->search(stopID);
            
            if (stop != nullptr) {
                // Show: ID - Name [Sector] (lat, lon)
                cout << stopID << " - " << stop->name;
                if (stop->sector[0] != '\0') {  // Show sector if available
                    cout << " [" << stop->sector << "]";
                }
                cout << " (" << stop->latitude << ", " << stop->longitude << ")";
            } else {
                // Fallback
                cout << stopID;
            }
            
            cout << endl;
        }
    }
    
    // grabbing stats
    int getBusCount() { return busCount; }
    int getStopCount() { return stopCount; }
    int getRouteEdgeCount() { return routeEdgeCount; }
    
    // Display all buses - NOW PROPERLY ITERATES HASHTABLE
    void displayAllBuses() {
        cout << "\n========== ALL REGISTERED BUSES ==========" << endl;
        cout << "Total Buses: " << busCount << endl;
        
        char** busKeys = new char*[100];
        int count;
        busTable->getAllKeys(busKeys, &count);
        
        if (count == 0) {
            cout << "\n[INFO] No buses registered yet." << endl;
        } else {
            cout << "\nNote: To see specific bus details, use 'Get Bus Details' option" << endl;
            cout << "Available buses: ";
            for (int i = 0; i < count; i++) {
                cout << busKeys[i];
                if (i < count - 1) cout << ", ";
            }
            cout << endl;
        }
        
        delete[] busKeys;
        cout << "==========================================" << endl;
    }
    
    // Display route summary
    void displayRouteSummary() {
        cout << "\n========== ROUTE NETWORK SUMMARY ==========" << endl;
        cout << "Total Bus Stops: " << stopCount << endl;
        cout << "Total Connections: " << routeEdgeCount << endl;
        cout << "Total Buses: " << busCount << endl;
        cout << "\nDetailed stop connections:" << endl;
        stopGraph->display();
        cout << "==========================================" << endl;
    }
    
    // ========== BONUS FEATURES ==========
    
    // bonus feature 1: School Bus Tracking
    HashTable* schoolBusTable = new HashTable(20);
    
    void registerSchoolBus(const char* busID, const char* schoolID) {
        SchoolBus* sb = new SchoolBus();
        stringCopyN(sb->busID, busID, 20);
        stringCopyN(sb->schoolID, schoolID, 50);
        schoolBusTable->insert(busID, sb);
        cout << "Registered school bus: " << busID << " for school " << schoolID << endl;
    }
    
    void addSchoolBusStop(const char* busID, const char* stopName) {
        SchoolBus* sb = (SchoolBus*)schoolBusTable->search(busID);
        if (sb == nullptr) {
            cout << "School bus not found!" << endl;
            return;
        }
        sb->studentStops->append(stopName);
        cout << "Added stop " << stopName << " to school bus " << busID << endl;
    }
    
    void trackSchoolBus(const char* busID, const char* currentLocation) {
        SchoolBus* sb = (SchoolBus*)schoolBusTable->search(busID);
        if (sb == nullptr) {
            cout << "School bus not found!" << endl;
            return;
        }
        
        // save previous location
        if (sb->currentLocation[0] != '\0') {
            sb->visitedStops->push(sb->currentLocation);
        }
        
        stringCopyN(sb->currentLocation, currentLocation, 50);
        cout << "School bus " << busID << " now at: " << currentLocation << endl;
        cout << "Students on board: " << sb->studentsOnBoard << "/" << sb->maxCapacity << endl;
    }
    
    void pickupStudent(const char* busID) {
        SchoolBus* sb = (SchoolBus*)schoolBusTable->search(busID);
        if (sb == nullptr) {
            cout << "School bus not found!" << endl;
            return;
        }
        if (sb->studentsOnBoard < sb->maxCapacity) {
            sb->studentsOnBoard++;
            cout << "Student boarded. Total: " << sb->studentsOnBoard << endl;
        } else {
            cout << "Bus is full!" << endl;
        }
    }
    
    // [NOTE] Airport and Railway systems are now in separate header files
    // AirportSector.h and RailwaySector.h
    
    void connectAirportToBusStop(const char* airportCode, const char* stopID, double distance) {
        stopGraph->addBidirectionalEdge(airportCode, stopID, distance);
        cout << "Connected airport " << airportCode << " to bus stop " << stopID << endl;
    }
    
    void connectRailToBusStop(const char* stationID, const char* stopID, double distance) {
        stopGraph->addBidirectionalEdge(stationID, stopID, distance);
        cout << "Connected rail station " << stationID << " to bus stop " << stopID << endl;
    }
    
    // bonus feature 3: Real-time Route Simulation
    int simulationStep = 0;
    bool simulationRunning = false;
    
    void startSimulation() {
        simulationRunning = true;
        simulationStep = 0;
        cout << "\n========== ROUTE SIMULATION STARTED ==========" << endl;
        cout << "Use simulateStep() to advance simulation" << endl;
    }
    
    void simulateStep() {
        if (!simulationRunning) {
            cout << "Simulation not started! Call startSimulation() first." << endl;
            return;
        }
        
        simulationStep++;
        cout << "\n--- Simulation Step " << simulationStep << " ---" << endl;
        
        // simulate all buses moving one step
        cout << "Simulating bus movements..." << endl;
        cout << "(In full implementation, buses would move to next stop)" << endl;
        cout << "Current step: " << simulationStep << endl;
    }
    
    void displayAllBusPositions() {
        cout << "\n========== CURRENT BUS POSITIONS ==========" << endl;
        cout << "Simulation Step: " << simulationStep << endl;
        cout << "\nRegular Buses:" << endl;
        cout << "(Bus position display would iterate through busTable)" << endl;
        
        cout << "\nSchool Buses:" << endl;
        cout << "(School bus positions tracked separately)" << endl;
    }
    
    void stopSimulation() {
        simulationRunning = false;
        cout << "\n========== SIMULATION STOPPED ==========" << endl;
        cout << "Total steps simulated: " << simulationStep << endl;
    }
    
    // bonus feature 4: Emergency Transport Routing
    HashTable* emergencyTable = new HashTable(20);
    MinHeap* emergencyQueue = new MinHeap(20);
    
    void registerEmergencyVehicle(const char* vehicleID, const char* type,
                                  const char* currentLocation, int priority) {
        EmergencyVehicle* ev = new EmergencyVehicle();
        stringCopyN(ev->vehicleID, vehicleID, 20);
        stringCopyN(ev->type, type, 50);
        stringCopyN(ev->currentLocation, currentLocation, 50);
        ev->priority = priority;
        ev->active = false;
        
        emergencyTable->insert(vehicleID, ev);
        cout << "Registered emergency vehicle: " << vehicleID << " (" << type << ")" << endl;
    }
    
    void dispatchEmergency(const char* vehicleID, const char* destination) {
        EmergencyVehicle* ev = (EmergencyVehicle*)emergencyTable->search(vehicleID);
        if (ev == nullptr) {
            cout << "Emergency vehicle not found!" << endl;
            return;
        }
        
        stringCopyN(ev->destination, destination, 50);
        ev->active = true;
        
        // add to priority queue (higher priority = lower value in min-heap)
        emergencyQueue->insert(ev, -ev->priority);
        
        cout << "\n*** EMERGENCY DISPATCH ***" << endl;
        cout << "Vehicle: " << ev->vehicleID << " (" << ev->type << ")" << endl;
        cout << "Priority: " << ev->priority << "/10" << endl;
        cout << "From: " << ev->currentLocation << endl;
        cout << "To: " << ev->destination << endl;
        
        // find emergency route
        findEmergencyRoute(ev->currentLocation, ev->destination, ev->priority);
    }
    
    void findEmergencyRoute(const char* from, const char* to, int priority) {
        cout << "\n--- EMERGENCY ROUTE (Priority " << priority << ") ---" << endl;
        
        // use existing dijkstra but mark it as emergency
        int vertexCount = stopGraph->getVertexCount();
        double* distances = new double[vertexCount];
        int* previous = new int[vertexCount];
        
        stopGraph->dijkstra(from, distances, previous);
        
        // CASE INSENSITIVE destination lookup
        int destIndex = -1;
        for (int i = 0; i < vertexCount; i++) {
            char vertexName[50];
            stringCopy(vertexName, stopGraph->getVertexName(i));
            toUpperCase(vertexName);
            
            char toUpper[50];
            stringCopy(toUpper, to);
            toUpperCase(toUpper);
            
            if (stringCompare(vertexName, toUpper) == 0) {
                destIndex = i;
                break;
            }
        }
        
        if (destIndex == -1 || distances[destIndex] == GRAPH_MAX_DISTANCE) {
            cout << "No route found!" << endl;
            delete[] distances;
            delete[] previous;
            return;
        }
        
        int path[100];
        int pathLength = 0;
        stopGraph->getPath(previous, destIndex, path, pathLength);
        
        cout << "*** EMERGENCY ROUTE ***" << endl;
        cout << "Distance: " << distances[destIndex] << " km" << endl;
        cout << "Path: ";
        
        for (int i = 0; i < pathLength; i++) {
            cout << stopGraph->getVertexName(path[i]);
            if (i < pathLength - 1) cout << " -> ";
        }
        cout << endl;
        cout << "!!! CLEAR THE ROUTE - EMERGENCY VEHICLE APPROACHING !!!" << endl;
        
        delete[] distances;
        delete[] previous;
    }
    
    void showActiveEmergencies() {
        cout << "\n========== ACTIVE EMERGENCIES ==========" << endl;
        if (emergencyQueue->isEmpty()) {
            cout << "No active emergencies" << endl;
        } else {
            cout << "Emergency vehicles in queue (by priority)" << endl;
            cout << "(Full implementation would display all from heap)" << endl;
        }
    }
    
    // bonus feature: Simulate Passenger Queue/Boarding at a stop (using Circular Queue)
    void simulatePassengerBoarding(const char* stopID) {
        BusStop* stop = (BusStop*)stopTable->search(stopID);
        if (stop == nullptr) {
            cout << "[ERROR] Stop not found!" << endl;
            return;
        }
        
        cout << "\n========== PASSENGER BOARDING SIMULATION ===========" << endl;
        cout << "Location: " << stop->name << " (" << stopID << ")" << endl;
        cout << "====================================================\n" << endl;
        
        // Create a circular queue for passengers (simulated with passenger IDs)
        Queue passengerQueue(20);
        
        // Simulate passengers arriving at the stop
        cout << "-- Passengers arriving at stop --" << endl;
        for (int i = 1; i <= 8; i++) {
            passengerQueue.enqueue(i);
            cout << "Passenger #" << i << " joined the queue" << endl;
        }
        
        cout << "\nTotal passengers waiting: " << passengerQueue.size() << endl;
        cout << "\n-- Bus arrives! Boarding begins --" << endl;
        
        int busCapacity = 5;
        int boarded = 0;
        
        while (!passengerQueue.isEmpty() && boarded < busCapacity) {
            int passenger = passengerQueue.dequeue();
            cout << "Passenger #" << passenger << " boards the bus" << endl;
            boarded++;
        }
        
        cout << "\n-- Boarding complete --" << endl;
        cout << "Passengers boarded: " << boarded << endl;
        cout << "Passengers still waiting: " << passengerQueue.size() << endl;
        
        if (!passengerQueue.isEmpty()) {
            cout << "\n[NOTE] " << passengerQueue.size() 
                 << " passenger(s) waiting for next bus" << endl;
            cout << "Next passenger in queue: #" << passengerQueue.peek() << endl;
        }
        
        cout << "\n=====================================================" << endl;
        cout << "[DEMO] Circular Queue operations demonstrated!" << endl;
        cout << "=====================================================" << endl;
    }
    
    // ========== VISUALIZATION HELPER METHODS ==========
    
    // Get all stop keys for visualization
    void getStopKeys(char** keys, int* count) {
        stopTable->getAllKeys(keys, count);
    }
    
    // Get stop by ID
    BusStop* getStop(const char* stopID) {
        return (BusStop*)stopTable->search(stopID);
    }
    
    // Get graph for pathfinding visualization
    Graph* getStopGraph() {
        return stopGraph;
    }
    
    // Get visual path between two stops (for visualization)
    // Returns path length, fills pathIds array with stop IDs, sets distance
    int getVisualPath(const char* from, const char* to, char pathIds[][50], double& distance) {
        int vertexCount = stopGraph->getVertexCount();
        double* distances = new double[vertexCount];
        int* previous = new int[vertexCount];
        
        stopGraph->dijkstra(from, distances, previous);
        
        // Find destination index
        int destIndex = -1;
        for (int i = 0; i < vertexCount; i++) {
            if (strcmp(stopGraph->getVertexName(i), to) == 0) {
                destIndex = i;
                break;
            }
        }
        
        if (destIndex == -1 || distances[destIndex] >= 999999) {
            delete[] distances;
            delete[] previous;
            distance = 0;
            return 0;  // No path found
        }
        
        // Get path
        int path[100];
        int pathLength = 0;
        stopGraph->getPath(previous, destIndex, path, pathLength);
        
        // Copy stop IDs to output
        for (int i = 0; i < pathLength && i < 50; i++) {
            strcpy(pathIds[i], stopGraph->getVertexName(path[i]));
        }
        
        distance = distances[destIndex];
        
        delete[] distances;
        delete[] previous;
        return pathLength;
    }
    
    // Get distance between two stops (returns 0 if not connected)
    double getDistance(const char* stop1, const char* stop2) {
        return stopGraph->getEdgeWeight(stop1, stop2);
    }
    
    // Bus visualization helpers
    void getBusKeys(char** keys, int* count) {
        busTable->getAllKeys(keys, count);
    }
    
    Bus* getBus(const char* busID) {
        return (Bus*)busTable->search(busID);
    }
    
    ~TransportSystem() {
        delete stopGraph;
        delete busTable;
        delete stopTable;
        delete routeHistory;
    }
};

#endif
