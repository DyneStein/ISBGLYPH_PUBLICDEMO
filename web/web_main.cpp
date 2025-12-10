#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>
#include "../include/Visualization.h"
#include "../include/CityMap.h"
#include "../include/PopulationSector.h"
#include "../include/TransportSector.h"
#include "../include/MedicalSector.h"
#include "../include/EducationSector.h"
#include "../include/CommercialSector.h"
#include "../include/PublicFacilities.h"

// Global Systems
PopulationSystem* popSys = nullptr;
TransportSystem* transSys = nullptr;
MedicalSystem* medSys = nullptr;
EducationSystem* eduSys = nullptr;
CommercialSystem* commSys = nullptr;
PublicFacilitySystem* pubSys = nullptr;
AirportSystem* airSys = nullptr;
RailwaySystem* railSys = nullptr;

Visualization* viz = nullptr;

// Main Loop Wrapper for Emscripten
void game_loop() {
    if (viz && viz->isRunning()) {
        viz->update();
        viz->render();
    }
}

// Data Loading (Hardcoded for demo or loaded from files if packed)
void loadDefaultData() {
    // 1. Population
    popSys->addSector("G-10");
    popSys->addSector("F-8");
    popSys->addSector("H-12");
    popSys->registerCitizen("37405-1111111-1", "Ali Khan", 25, "G-10", "Street 1", "Male", "Engineer", "Male");
    
    // 2. Transport
    transSys->addBusStop("STOP1", "G-10 Markaz", "G-10", 33.6, 73.0);
    transSys->addBusStop("STOP2", "F-8 Markaz", "F-8", 33.7, 73.1);
    transSys->registerBus("B-101", "Red Line", "Metro");
    transSys->addStopToRoute("B-101", "STOP1");
    transSys->addStopToRoute("B-101", "STOP2");
    
    // 3. Medical
    medSys->registerHospital("H-01", "PIMS", "G-8", 500, "General");
    medSys->registerPharmacy("PH-01", "D.Watson", "Blue Area");
    
    // 4. Education
    eduSys->registerSchool("S-01", "Beaconhouse", "H-8", 4.5, "Private");
    
    // 5. Commercial
    commSys->registerMall("M-01", "Centaurus", "F-8", 5);
    
    // 6. Public
    pubSys->addPark("P-01", "Fatima Jinnah Park", "F-9");
}

int main() {
    std::cout << "Starting Web Assembly Ver..." << std::endl;
    
    // Initialize Systems
    popSys = new PopulationSystem();
    transSys = new TransportSystem(popSys);
    medSys = new MedicalSystem(popSys);
    eduSys = new EducationSystem(popSys);
    commSys = new CommercialSystem(popSys);
    pubSys = new PublicFacilitySystem(popSys);
    airSys = new AirportSystem(transSys);
    railSys = new RailwaySystem(transSys); // Fixed constructor dependency if needed

    // Load Data
    loadDefaultData();
    
    // Initialize Visualization
    viz = new Visualization();
    if (!viz->init("assets")) {
        std::cerr << "Failed to load assets!" << std::endl;
        return 1;
    }
    
    // Link Systems to Visualization
    viz->collectAllSectors(popSys, transSys, medSys, eduSys);
    viz->setTransportSystem(transSys);
    viz->setMedicalSystem(medSys);
    viz->setEducationSystem(eduSys);
    viz->setCommercialSystem(commSys);
    viz->setPublicFacilities(pubSys);
    viz->setAirportSystem(airSys);
    viz->setRailwaySystem(railSys);
    
    // Start Loop
    emscripten_set_main_loop(game_loop, 0, 1);
    
    return 0;
}
