#ifndef TRANSPORTMENUS_H
#define TRANSPORTMENUS_H

#include <iostream>
using namespace std;

// Forward declarations
class AirportSystem;
class RailwaySystem;

extern AirportSystem* airportSystem;
extern RailwaySystem* railwaySystem;

// Airport System Menu
void airportMenu() {
    int choice = -1;
    do {
        cout << "\n+--------------------------------------------+" << endl;
        cout << "|   AIRPORT SYSTEM MENU                      |" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << "  [1] Display All Airports" << endl;
        cout << "  [2] Register New Airport" << endl;
        cout << "  [3] Display All Flights" << endl;
        cout << "  [4] Add Flight" << endl;
        cout << "  [5] Find Nearest Airport" << endl;
        cout << "  [6] Connect Airports (Create Route)" << endl;
        cout << "  [7] Find Shortest Path Between Airports" << endl;
        cout << "  [8] Display Route Network Summary" << endl;
        cout << "  [9] Simulate Passenger Boarding Queue" << endl;
        cout << "  [0] <-- Back to Transport Menu" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << ">> Stats: " << airportSystem->getAirportCount() << " airports, "
             << airportSystem->getFlightCount() << " flights" << endl;
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
            airportSystem->displayAllAirports();
            
        } else if (choice == 2) {
            airportSystem->registerAirportManual();
            
        } else if (choice == 3) {
            airportSystem->displayAllFlights();
            
        } else if (choice == 4) {
            airportSystem->addFlightManual();
            
        } else if (choice == 5) {
            airportSystem->findNearestAirport();
            
        } else if (choice == 6) {
            airportSystem->connectAirportsManual();
            
        } else if (choice == 7) {
            airportSystem->findShortestPath();
            
        } else if (choice == 8) {
            airportSystem->displayRouteNetworkSummary();
            
        } else if (choice == 9) {
            airportSystem->simulatePassengerBoarding();
            
        } else if (choice != 0) {
            cout << "[ERROR] Invalid choice!" << endl;
        }
    } while (choice != 0);
}

// Railway System Menu
void railwayMenu() {
    int choice = -1;
    do {
        cout << "\n+--------------------------------------------+" << endl;
        cout << "|   RAILWAY SYSTEM MENU                      |" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << "  [1] Display All Stations" << endl;
        cout << "  [2] Register New Station" << endl;
        cout << "  [3] Display All Trains" << endl;
        cout << "  [4] Add Train" << endl;
        cout << "  [5] Find Nearest Station" << endl;
        cout << "  [6] Connect Stations (Create Route)" << endl;
        cout << "  [7] Find Shortest Path Between Stations" << endl;
        cout << "  [8] Display Route Network Summary" << endl;
        cout << "  [9] Simulate Passenger Boarding Queue" << endl;
        cout << "  [0] <-- Back to Transport Menu" << endl;
        cout << "+--------------------------------------------+" << endl;
        cout << ">> Stats: " << railwaySystem->getStationCount() << " stations, "
             << railwaySystem->getTrainCount() << " trains" << endl;
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
            railwaySystem->displayAllStations();
            
        } else if (choice == 2) {
            railwaySystem->registerStationManual();
            
        } else if (choice == 3) {
            railwaySystem->displayAllTrains();
            
        } else if (choice == 4) {
            railwaySystem->addTrainManual();
            
        } else if (choice == 5) {
            railwaySystem->findNearestStation();
            
        } else if (choice == 6) {
            railwaySystem->connectStationsManual();
            
        } else if (choice == 7) {
            railwaySystem->findShortestPath();
            
        } else if (choice == 8) {
            railwaySystem->displayRouteNetworkSummary();
            
        } else if (choice == 9) {
            railwaySystem->simulatePassengerBoarding();
            
        } else if (choice != 0) {
            cout << "[ERROR] Invalid choice!" << endl;
        }
    } while (choice != 0);
}

#endif
