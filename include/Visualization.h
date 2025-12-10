#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <SFML/Graphics.hpp>
#include <cstring>
#include <cstdio>
#include <cmath>
#include "CityMap.h"
#include "CityRenderer.h"
#include "TransportSector.h"
#include "MedicalSector.h"
#include "EducationSector.h"
#include "PopulationSector.h"
#include "AirportSector.h"
#include "RailwaySector.h"
#include "CommercialSector.h"
#include "PublicFacilities.h"
#include "Utils.h"

const float UI_WIDTH = 250.0f;

// Entity data for rendering
struct StopData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct HospitalData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct SchoolData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct PharmacyData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct MallData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct ParkData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct AirportData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct RailwayStationData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct MosqueData {
    char id[50];
    char name[100];
    char sector[50];
    float x, y;
};

struct ConnectionData {
    int fromIdx, toIdx;
    float distance;
};

struct BusData {
    char id[20];
    char name[100];
    char currentStop[50];
    float x, y;
    // Animation state
    float animX, animY;      // Current animated position
    float targetX, targetY;  // Target position (next stop)
    float progress;          // 0.0 to 1.0 interpolation
    float pauseTimer;        // Pause at stops (seconds)
    bool isMoving;           // Currently in motion
    // Path following
    int pathStops[20];       // Stop indices in path
    int pathLength;          // Number of stops in path
    int pathIndex;           // Current position in path
    int currentStopIdx;      // Current stop index
};

struct TrainData {
    char id[20];
    char name[100];
    char currentStation[50];
    float x, y;
    // Animation state
    float animX, animY;
    float targetX, targetY;
    float progress;
    float pauseTimer;
    bool isMoving;
    // Path following
    int pathStations[10];
    int pathLength;
    int pathIndex;
    int currentStationIdx;
};

struct FlightData {
    char id[20];
    char airline[100];
    char currentAirport[50];
    float x, y;
    // Animation state
    float animX, animY;
    float targetX, targetY;
    float progress;
    float pauseTimer;
    bool isMoving;
    // Path following
    int pathAirports[10];
    int pathLength;
    int pathIndex;
    int currentAirportIdx;
};

// Decorative elements (not linked to any system)
struct DecorationData {
    int type;         // 0=house, 1=flats, 2=factory, 3=water tower, 4=restaurant, 5=gas station, 6=police, 7=fire
    float x, y;
    char sector[20];
};

class Visualization {
private:
    sf::RenderWindow window;
    sf::Font font;
    CityMap cityMap;
    CityRenderer renderer;
    
    // Camera
    float camX, camY, zoom;
    
    // Animation
    sf::Clock animationClock;
    bool animationPaused;
    float vehicleSpeed;  // World units per second
    
    // Entity arrays
    StopData stops[50];
    int stopCount;
    HospitalData hospitals[20];
    int hospitalCount;
    SchoolData schools[20];
    int schoolCount;
    PharmacyData pharmacies[20];
    int pharmacyCount;
    MallData malls[20];
    int mallCount;
    ParkData parks[20];
    int parkCount;
    AirportData airports[10];
    int airportCount;
    RailwayStationData railwayStations[10];
    int railwayStationCount;
    ConnectionData connections[100];
    int connectionCount;
    ConnectionData airportConnections[50];
    int airportConnectionCount;
    ConnectionData railwayConnections[50];
    int railwayConnectionCount;
    
    // Vehicle arrays
    BusData buses[10];
    int busCount;
    TrainData trains[20];
    int trainCount;
    FlightData flights[20];
    int flightCount;
    MosqueData mosques[20];
    int mosqueCount;
    
    // Decorative elements (not linked to systems)
    DecorationData decorations[200];  // Up to 200 decorations (50 sectors * 4)
    int decorationCount;

    // Animated People
    struct PersonData {
        int type;       // 0=Citizen(M), 1=Citizen(F), 2=Police, 3=Doctor, 4=Student, 5=Engineer
        float x, y;     // Current position
        float targetX, targetY; // Destination
        float baseX, baseY;     // Center of assigned building (for context-aware)
        int state;      // 0=Roaming, 1=Entering, 2=Exiting
        float opacity;  // For fade effect (0.0 - 255.0)
        float speed;
        float waitTimer; // Time to wait inside
    };

    PersonData people[500]; // Max 500 people
    int personCount;
    bool showPeople;

    // People Textures
    sf::Texture texPersonM, texPersonF, texPolice, texDoctor, texStudent, texEngineer;
    
    // Selection tracking
    int selectedStop;
    int selectedHospital;
    int selectedSchool;
    int selectedPharmacy;
    int selectedMall;
    int selectedPark;
    int selectedMosque;
    int selectedAirport;
    int selectedRailwayStation;
    int selectedBus;
    int selectedTrain;
    int selectedFlight;
    char currentSector[50];
    
    // Spatial grid for collision-free placement
    struct OccupiedPosition {
        float x, y;
        float radius;
    };
    
    struct SectorSpatialData {
        OccupiedPosition positions[100];
        int count;
        char sectorName[50];
        
        SectorSpatialData() : count(0) {
            sectorName[0] = '\0';
        }
    };
    
    SectorSpatialData sectorGrids[20];  // One per sector
    int sectorGridCount;
    
    // Search/Filter system
    struct SearchResult {
        int entityType;     // 0=stop, 1=hospital, 2=school, etc.
        int index;          // Index in entity array
        char name[100];
        char id[50];
        float x, y;         // Position for navigation
        int matchScore;     // Relevance (100=exact, 75=starts-with, 50=contains)
    };
    
    char searchQuery[100];
    SearchResult searchResults[50];
    int searchResultCount;
    int selectedSearchResult;     // -1 = none, 0+ = index in searchResults
    bool searchBoxFocused;
    int searchFilterType;          // -1 = all, 0-11 = specific type
    int hoveredSearchResult;       // For mouse hover
    
    // Pathfinding visualization
    bool pathfindingMode;          // Are we selecting path endpoints?
    int pathfindingNetworkType;    // 0=bus stops, 1=airports, 2=railways
    int pathSourceStop;            // Index of source stop (-1 = not selected)
    int pathDestStop;              // Index of destination stop (-1 = not selected)
    char pathStopIds[50][50];      // IDs of stops in the calculated path
    int pathStopCount;             // Number of stops in path
    float pathDistance;            // Total path distance in km
    bool pathFound;                // Was a valid path found?
    
    // Toggles
    bool showConnections;
    bool showAirportConnections;
    bool showRailwayConnections;
    int connectionCycleState;  // 0-5: tracks '1' key cycle position
    bool showHospitals;
    bool showSchools;
    bool showPharmacies;
    bool showMalls;
    bool showParks;
    bool showAirports;
    bool showRailways;
    bool showBuses;
    bool showTrains;
    bool showFlights;
    bool showMosques;
    bool showDecorations;
    bool showInstructions;
    bool running;
    
    // System references
    TransportSystem* transport;
    MedicalSystem* medical;
    EducationSystem* education;
    AirportSystem* airports_sys;
    RailwaySystem* railway_sys;
    CommercialSystem* commercial;
    PublicFacilitySystem* publicFacilities;
    
    // Optimisation: Vertex Arrays for Batch Rendering
    sf::VertexArray vaPeople[6]; // 0=M, 1=F, 2=Police, 3=Doc, 4=Student, 5=Eng
    sf::VertexArray vaDecorations[15]; // Store batched vertices for each decoration type
    
public:
    Visualization() : window(sf::VideoMode({1920u, 1080u}), "ISBGlyph - Islamabad"),
                      camX(0), camY(0), zoom(1.0f), 
                      stopCount(0), hospitalCount(0), schoolCount(0),
                      pharmacyCount(0), mallCount(0), parkCount(0),
                      airportCount(0), railwayStationCount(0), connectionCount(0),
                      personCount(0), showPeople(true),
                      airportConnectionCount(0), railwayConnectionCount(0),
                      busCount(0), trainCount(0), flightCount(0), mosqueCount(0), decorationCount(0),
                      selectedStop(-1), selectedHospital(-1), selectedSchool(-1),
                      selectedPharmacy(-1), selectedMall(-1), selectedPark(-1),
                      selectedMosque(-1), selectedAirport(-1), selectedRailwayStation(-1),
                      selectedBus(-1), selectedTrain(-1), selectedFlight(-1),
                      sectorGridCount(0),
                      searchResultCount(0), selectedSearchResult(-1),
                      searchBoxFocused(false), searchFilterType(-1), hoveredSearchResult(-1),
                      pathfindingMode(false), pathfindingNetworkType(0),
                      pathSourceStop(-1), pathDestStop(-1),
                      pathStopCount(0), pathDistance(0.0f), pathFound(false),
                      showConnections(true), showAirportConnections(true), showRailwayConnections(true),
                      connectionCycleState(0),
                      showHospitals(true), showSchools(true),
                      showPharmacies(true), showMalls(true), showParks(true),
                      showAirports(true), showRailways(true),
                      showBuses(true), showTrains(true), showFlights(true), showMosques(true), showDecorations(true), showInstructions(false),
                      animationPaused(true), vehicleSpeed(150.0f),  // Paused by default
                      running(true), 
                      transport(nullptr), medical(nullptr), education(nullptr),
                      airports_sys(nullptr), railway_sys(nullptr), commercial(nullptr),
                      publicFacilities(nullptr) {
        window.setFramerateLimit(60);
        strcpy(currentSector, "None");
        searchQuery[0] = '\0';  // Initialize empty search
        
        // Initialize batch rendering arrays
        for(int i = 0; i < 6; i++) vaPeople[i].setPrimitiveType(sf::PrimitiveType::Triangles);
        for(int i = 0; i < 15; i++) vaDecorations[i].setPrimitiveType(sf::PrimitiveType::Triangles);
    }
    
    bool init(const char* assetPath) {
        char fontPath[256];
        snprintf(fontPath, 256, "%s/fonts/arial.ttf", assetPath);
        if (!font.openFromFile(fontPath)) {
            printf("[ERROR] Could not load font\n");
            return false;
        }
        
        if (!renderer.loadTextures(assetPath)) {
            printf("[ERROR] Could not load textures\n");
            return false;
        }
        
        printf("[OK] Visualization initialized\n");
        return true;
    }
    
    // Collect ALL sectors from ALL data sources
    void collectAllSectors(PopulationSystem* pop, TransportSystem* trans, 
                          MedicalSystem* med, EducationSystem* edu) {
        char* allSectors[100];
        int totalCount = 0;
        
        // Helper to add sector if unique (case-insensitive, whitespace-trimmed)
        auto addUniqueSector = [&](const char* sector) {
            if (sector == nullptr || sector[0] == '\0') return;
            
            // Trim and normalize input sector
            char trimmed[100];
            int start = 0;
            while (sector[start] == ' ') start++;
            
            int len = strlen(sector + start);
            while (len > 0 && sector[start + len - 1] == ' ') len--;
            
            for (int i = 0; i < len; i++) {
                trimmed[i] = sector[start + i];
            }
            trimmed[len] = '\0';
            
            if (trimmed[0] == '\0') return; // Empty after trimming
            
            // Normalize to uppercase for comparison
            char normalizedSector[100];
            strcpy(normalizedSector, trimmed);
            toUpperCase(normalizedSector);
            
            // Check if already exists (case-insensitive)
            for (int i = 0; i < totalCount; i++) {
                char existingNormalized[100];
                strcpy(existingNormalized, allSectors[i]);
                toUpperCase(existingNormalized);
                
                if (strcmp(existingNormalized, normalizedSector) == 0) {
                    return; // Already exists
                }
            }
            
            // Add new unique sector (using trimmed original case)
            if (totalCount < 100) {
                int trimmedLen = strlen(trimmed);
                allSectors[totalCount] = new char[trimmedLen + 1];
                strcpy(allSectors[totalCount], trimmed);
                totalCount++;
            }
        };
        
        // 1. Collect from PopulationSystem
        if (pop) {
            char** popSectors = new char*[50];
            int popCount = 0;
            pop->getSectorNames(popSectors, &popCount);
            for (int i = 0; i < popCount; i++) {
                addUniqueSector(popSectors[i]);
                delete[] popSectors[i];
            }
            delete[] popSectors;
        }
        
        // 2. Collect from TransportSystem (bus stops)
        if (trans) {
            char** keys = new char*[50];
            int count = 0;
            trans->getStopKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                BusStop* stop = trans->getStop(keys[i]);
                if (stop) {
                    addUniqueSector(stop->sector);
                }
            }
            delete[] keys;
        }
        
        // 3. Collect from MedicalSystem (hospitals)
        if (med) {
            char** keys = new char*[20];
            int count = 0;
            med->getHospitalKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                Hospital* hosp = med->getHospital(keys[i]);
                if (hosp) {
                    addUniqueSector(hosp->sector);
                }
            }
            delete[] keys;
        }
        
        // 4. Collect from EducationSystem (schools)
        if (edu) {
            char** keys = new char*[20];
            int count = 0;
            edu->getSchoolKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                School* school = edu->getSchool(keys[i]);
                if (school) {
                    addUniqueSector(school->sector);
                }
            }
            delete[] keys;
        }
        
        // 5. Collect from MedicalSystem (pharmacies)
        if (med) {
            char** keys = new char*[20];
            int count = 0;
            med->getPharmacyKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                Pharmacy* pharm = med->getPharmacy(keys[i]);
                if (pharm) {
                    addUniqueSector(pharm->sector);
                }
            }
            delete[] keys;
        }
        
        // 6. Collect from CommercialSystem (malls)
        if (commercial) {
            char** keys = new char*[20];
            int count = 0;
            commercial->getMallKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                Mall* mall = commercial->getMall(keys[i]);
                if (mall) {
                    addUniqueSector(mall->sector);
                }
            }
            delete[] keys;
        }
        
        // 6. Collect from PublicFacilitySystem (parks AND mosques)
        if (publicFacilities) {
            char** keys = new char*[20];
            int count = 0;
            
            // Parks
            publicFacilities->getParkKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                Facility* park = publicFacilities->getPark(keys[i]);
                if (park) {
                    addUniqueSector(park->sector);
                }
            }
            
            // Mosques
            publicFacilities->getMosqueKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                Facility* mosque = publicFacilities->getMosque(keys[i]);
                if (mosque) {
                    addUniqueSector(mosque->sector);
                }
            }
            
            delete[] keys;
        }
        
        // 8. Collect from AirportSystem
        if (airports_sys) {
            char** keys = new char*[10];
            int count = 0;
            airports_sys->getAirportKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                Airport* airport = airports_sys->getAirport(keys[i]);
                if (airport) {
                    addUniqueSector(airport->sector);
                }
            }
            delete[] keys;
        }
        
        // 9. Collect from RailwaySystem
        if (railway_sys) {
            char** keys = new char*[10];
            int count = 0;
            railway_sys->getStationKeys(keys, &count);
            for (int i = 0; i < count; i++) {
                RailStation* station = railway_sys->getStation(keys[i]);
                if (station) {
                    addUniqueSector(station->sector);
                }
            }
            delete[] keys;
        }
        
        // Build city map from collected unique sectors
        if (totalCount > 0) {
            cityMap.buildGridFromSectors(allSectors, totalCount);
            printf("[OK] Collected %d unique sectors from all data sources\n", totalCount);
        } else {
            printf("[WARNING] No sectors found in any system\n");
        }
        
        // Clean up
        for (int i = 0; i < totalCount; i++) {
            delete[] allSectors[i];
        }
    }
    
    void setTransportSystem(TransportSystem* ts) { 
        transport = ts; 
        loadStopsFromSystem();
        loadConnectionsFromSystem();
    }
    
    void setMedicalSystem(MedicalSystem* ms) { 
        medical = ms;
        loadHospitalsFromSystem();
    }
    
    void setEducationSystem(EducationSystem* es) { 
        education = es;
        loadSchoolsFromSystem();
    }
    
    void setCommercialSystem(CommercialSystem* cs) {
        commercial = cs;
    }
    
    void setPublicFacilities(PublicFacilitySystem* pfs) {
        publicFacilities = pfs;
    }
    
    void setAirportSystem(AirportSystem* as) {
        airports_sys = as;
    }
    
    void setRailwaySystem(RailwaySystem* rs) {
        railway_sys = rs;
    }
    
    // ========== COLLISION-FREE PLACEMENT HELPERS ==========
    
    // Clear all spatial grids (call before reload)
    void clearSectorGrids() {
        sectorGridCount = 0;
        for (int i = 0; i < 20; i++) {
            sectorGrids[i].count = 0;
            sectorGrids[i].sectorName[0] = '\0';
        }
    }
    
    // Normalize sector name for comparison (uppercase, trimmed)
    void normalizeSectorForGrid(const char* src, char* dst) {
        int i = 0, j = 0;
        // Skip leading whitespace
        while (src[i] == ' ') i++;
        // Copy and uppercase
        while (src[i] != '\0' && j < 49) {
            if (src[i] >= 'a' && src[i] <= 'z') {
                dst[j++] = src[i] - 32;  // Convert to uppercase
            } else {
                dst[j++] = src[i];
            }
            i++;
        }
        // Trim trailing whitespace
        while (j > 0 && dst[j-1] == ' ') j--;
        dst[j] = '\0';
    }
    
    // Get or create spatial grid for a sector (case-insensitive)
    SectorSpatialData* getSectorGrid(const char* sector) {
        // Normalize input sector name
        char normalizedInput[50];
        normalizeSectorForGrid(sector, normalizedInput);
        
        // Find existing (case-insensitive)
        for (int i = 0; i < sectorGridCount; i++) {
            char normalizedExisting[50];
            normalizeSectorForGrid(sectorGrids[i].sectorName, normalizedExisting);
            if (strcmp(normalizedExisting, normalizedInput) == 0) {
                return &sectorGrids[i];
            }
        }
        
        // Create new
        if (sectorGridCount < 20) {
            strcpy(sectorGrids[sectorGridCount].sectorName, sector);
            sectorGrids[sectorGridCount].count = 0;
            return &sectorGrids[sectorGridCount++];
        }
        
        return nullptr;
    }
    
    // Check if position collides with existing entities
    bool checkCollision(SectorSpatialData* grid, float x, float y, float radius) {
        if (!grid) return false;
        
        for (int i = 0; i < grid->count; i++) {
            float dx = x - grid->positions[i].x;
            float dy = y - grid->positions[i].y;
            float dist = std::sqrt(dx*dx + dy*dy);
            float minDist = radius + grid->positions[i].radius + 20.0f;  // 20px buffer
            
            if (dist < minDist) {
                return true;  // Collision!
            }
        }
        
        return false;  // No collision
    }
    
    // Generate spiral candidate positions from center outward
    void generateSpiralPositions(float* offsetsX, float* offsetsY, int& count) {
        count = 0;
        
        // Center
        offsetsX[count] = 0.0f; offsetsY[count++] = 0.0f;
        
        // Spiral outward in rings
        float baseSpacing = 90.0f;  // Reduced from 180px to 90px
        
        // Ring 1 (8 positions around center)
        for (int angle = 0; angle < 360; angle += 45) {
            float rad = angle * 3.14159f / 180.0f;
            offsetsX[count] = std::cos(rad) * baseSpacing;
            offsetsY[count++] = std::sin(rad) * baseSpacing;
        }
        
        // Ring 2 (16 positions)
        for (int angle = 0; angle < 360; angle += 22) {
            float rad = angle * 3.14159f / 180.0f;
            offsetsX[count] = std::cos(rad) * baseSpacing * 2.0f;
            offsetsY[count++] = std::sin(rad) * baseSpacing * 2.0f;
            if (count >= 100) return;
        }
        
        // Ring 3 (24 positions)
        for (int angle = 0; angle < 360; angle += 15) {
            float rad = angle * 3.14159f / 180.0f;
            offsetsX[count] = std::cos(rad) * baseSpacing * 3.0f;
            offsetsY[count++] = std::sin(rad) * baseSpacing * 3.0f;
            if (count >= 100) return;
        }
        
        // Ring 4 (fill remaining)
        for (int angle = 0; angle < 360 && count < 100; angle += 10) {
            float rad = angle * 3.14159f / 180.0f;
            offsetsX[count] = std::cos(rad) * baseSpacing * 4.0f;
            offsetsY[count++] = std::sin(rad) * baseSpacing * 4.0f;
        }
    }
    
    // Find free position in sector that doesn't collide
    bool findFreePosition(const char* sector, float radius, float& outX, float& outY) {
        SectorSpatialData* grid = getSectorGrid(sector);
        if (!grid) return false;  // Exceed sector limit
        
        // 1. Get sector dimensions and center
        float cx, cy, w, h;
        if (!cityMap.getSectorPosition(sector, cx, cy)) return false;
        cityMap.getSectorDimensions(sector, w, h);
        
        // 2. SAFETY CHECK: Define strict bounds
        // Entity must be fully inside sector (center +/- radius)
        // We add a 5px margin for aesthetics so it doesn't touch the border line
        float margin = 5.0f;
        float safeHalfW = (w / 2.0f) - radius - margin;
        float safeHalfH = (h / 2.0f) - radius - margin;
        
        // If entity is too big for the sector, we can't place it safely
        if (safeHalfW < 0) safeHalfW = 0;
        if (safeHalfH < 0) safeHalfH = 0;
        
        // 3. Try Spiral Candidates FIRST
        float offsetsX[100], offsetsY[100];
        int candidateCount;
        generateSpiralPositions(offsetsX, offsetsY, candidateCount);
        
        for (int i = 0; i < candidateCount; i++) {
            float testX, testY;
            // Note: getPositionInSector applies scaling based on sector growth
            cityMap.getPositionInSector(sector, offsetsX[i], offsetsY[i], testX, testY);
            
            // STRICT BOUNDARY CHECK
            // Check if this candidate puts the entity out of bounds
            if (std::abs(testX - cx) > safeHalfW) continue; // Out of width bounds
            if (std::abs(testY - cy) > safeHalfH) continue; // Out of height bounds
            
            // Check collision with other entities
            if (!checkCollision(grid, testX, testY, radius)) {
                // Found free position!
                outX = testX;
                outY = testY;
                return true;
            }
        }
        
        // 4. FALLBACK: Monte Carlo Sampling
        // If spiral didn't work (maybe because spiral points were out of bounds or occupied),
        // try random positions STRICTLY within the safe zone.
        int attempts = 200; // Try 200 random spots
        for (int i = 0; i < attempts; i++) {
            // Generate random offset within the SAFE zone
            float rndRelX = 0;
            float rndRelY = 0;
            
            if (safeHalfW > 1.0f) {
                rndRelX = (float)((rand() % (int)(safeHalfW * 2)) - safeHalfW);
            }
            if (safeHalfH > 1.0f) {
                rndRelY = (float)((rand() % (int)(safeHalfH * 2)) - safeHalfH);
            }
            
            float testX = cx + rndRelX;
            float testY = cy + rndRelY;
            
            // Collision check
            if (!checkCollision(grid, testX, testY, radius)) {
                outX = testX;
                outY = testY;
                return true;
            }
        }
        
        // Could not find any valid spot
        return false;
    }
    
    // Mark position as occupied
    void markPositionOccupied(const char* sector, float x, float y, float radius) {
        SectorSpatialData* grid = getSectorGrid(sector);
        if (!grid || grid->count >= 100) return;
        
        grid->positions[grid->count].x = x;
        grid->positions[grid->count].y = y;
        grid->positions[grid->count].radius = radius;
        grid->count++;
    }
    
    // ========== SEARCH/FILTER SYSTEM ==========
    
    // Convert string to lowercase for case-insensitive matching
    void toLowercase(const char* src, char* dst) {
        int i = 0;
        while (src[i] != '\0' && i < 99) {
            dst[i] = std::tolower(src[i]);
            i++;
        }
        dst[i] = '\0';
    }
    
    // Calculate match score for search
    int matchScore(const char* text, const char* query) {
        if (query[0] == '\0') return 0;  // Empty query
        
        char lowerText[100], lowerQuery[100];
        toLowercase(text, lowerText);
        toLowercase(query, lowerQuery);
        
        // Exact match
        if (strcmp(lowerText, lowerQuery) == 0) return 100;
        
        // Starts with
        if (strstr(lowerText, lowerQuery) == lowerText) return 75;
        
        // Contains
        if (strstr(lowerText, lowerQuery) != nullptr) return 50;
        
        return 0;  // No match
    }
    
    // Update search results based on current query
    void updateSearch() {
        searchResultCount = 0;
        if (searchQuery[0] == '\0') return;  // Empty query
        
        // Helper macro to add result
        #define ADD_RESULT(type, idx, entityName, entityId, entityX, entityY) \
            if (searchResultCount < 50) { \
                int nameScore = matchScore(entityName, searchQuery); \
                int idScore = matchScore(entityId, searchQuery); \
                int score = std::max(nameScore, idScore); \
                if (score > 0 && (searchFilterType == -1 || searchFilterType == type)) { \
                    searchResults[searchResultCount].entityType = type; \
                    searchResults[searchResultCount].index = idx; \
                    strcpy(searchResults[searchResultCount].name, entityName); \
                    strcpy(searchResults[searchResultCount].id, entityId); \
                    searchResults[searchResultCount].x = entityX; \
                    searchResults[searchResultCount].y = entityY; \
                    searchResults[searchResultCount].matchScore = score; \
                    searchResultCount++; \
                } \
            }
        
        // Search all entity types
        for (int i = 0; i < stopCount; i++) {
            ADD_RESULT(0, i, stops[i].name, stops[i].id, stops[i].x, stops[i].y);
        }
        for (int i = 0; i < hospitalCount; i++) {
            ADD_RESULT(1, i, hospitals[i].name, hospitals[i].id, hospitals[i].x, hospitals[i].y);
        }
        for (int i = 0; i < schoolCount; i++) {
            ADD_RESULT(2, i, schools[i].name, schools[i].id, schools[i].x, schools[i].y);
        }
        for (int i = 0; i < pharmacyCount; i++) {
            ADD_RESULT(3, i, pharmacies[i].name, pharmacies[i].id, pharmacies[i].x, pharmacies[i].y);
        }
        for (int i = 0; i < mallCount; i++) {
            ADD_RESULT(4, i, malls[i].name, malls[i].id, malls[i].x, malls[i].y);
        }
        for (int i = 0; i < parkCount; i++) {
            ADD_RESULT(5, i, parks[i].name, parks[i].id, parks[i].x, parks[i].y);
        }
        for (int i = 0; i < mosqueCount; i++) {
            ADD_RESULT(6, i, mosques[i].name, mosques[i].id, mosques[i].x, mosques[i].y);
        }
        for (int i = 0; i < airportCount; i++) {
            ADD_RESULT(7, i, airports[i].name, airports[i].id, airports[i].x, airports[i].y);
        }
        for (int i = 0; i < railwayStationCount; i++) {
            ADD_RESULT(8, i, railwayStations[i].name, railwayStations[i].id, railwayStations[i].x, railwayStations[i].y);
        }
        for (int i = 0; i < busCount; i++) {
            ADD_RESULT(9, i, buses[i].name, buses[i].id, buses[i].x, buses[i].y);
        }
        for (int i = 0; i < trainCount; i++) {
            ADD_RESULT(10, i, trains[i].name, trains[i].id, trains[i].x, trains[i].y);
        }
        for (int i = 0; i < flightCount; i++) {
            ADD_RESULT(11, i, flights[i].airline, flights[i].id, flights[i].x, flights[i].y);
        }
        
        #undef ADD_RESULT
        
        // Sort results by match score (bubble sort for simplicity)
        for (int i = 0; i < searchResultCount - 1; i++) {
            for (int j = 0; j < searchResultCount - i - 1; j++) {
                if (searchResults[j].matchScore < searchResults[j + 1].matchScore) {
                    SearchResult temp = searchResults[j];
                    searchResults[j] = searchResults[j + 1];
                    searchResults[j + 1] = temp;
                }
            }
        }
        
        // Reset selection
        selectedSearchResult = -1;
        hoveredSearchResult = -1;
    }
    
    // Navigate to and select a search result
    void selectSearchResult(int resultIndex) {
        if (resultIndex < 0 || resultIndex >= searchResultCount) return;
        
        SearchResult& result = searchResults[resultIndex];
        
        // Pan camera to entity
        camX = -(result.x - 960.0f);
        camY = -(result.y - 540.0f);
        
        // Auto-zoom to 150% for visibility
        zoom = 1.5f;
        
        // Select the entity based on type
        // Clear all selections first
        selectedStop = selectedHospital = selectedSchool = -1;
        selectedPharmacy = selectedMall = selectedPark = selectedMosque = -1;
        selectedAirport = selectedRailwayStation = -1;
        selectedBus = selectedTrain = selectedFlight = -1;
        
        // Select the found entity
        switch (result.entityType) {
            case 0: selectedStop = result.index; break;
            case 1: selectedHospital = result.index; break;
            case 2: selectedSchool = result.index; break;
            case 3: selectedPharmacy = result.index; break;
            case 4: selectedMall = result.index; break;
            case 5: selectedPark = result.index; break;
            case 6: selectedMosque = result.index; break;
            case 7: selectedAirport = result.index; break;
            case 8: selectedRailwayStation = result.index; break;
            case 9: selectedBus = result.index; break;
            case 10: selectedTrain = result.index; break;
            case 11: selectedFlight = result.index; break;
        }
        
        // Close search
        searchBoxFocused = false;
        selectedSearchResult = -1;
    }
    
    // ========== DECORATIVE ELEMENTS ==========
    
    void generateSectorDecorations() {
        decorationCount = 0;
        
        // Get all sectors from cityMap
        for (int s = 0; s < cityMap.getSectorCount() && decorationCount < 200; s++) {
            const char* sectorName = cityMap.getSector(s)->name;
            
            // Generate 3-4 decorations per sector
            int numDecorations = 3 + (rand() % 2);  // 3 or 4
            
            for (int i = 0; i < numDecorations && decorationCount < 200; i++) {
                // Pick random decoration type (0-7)
                int type = rand() % 8;
                
                float x, y;
                // Use collision-free placement (60px radius for decorations)
                if (findFreePosition(sectorName, 60.0f, x, y)) {
                    decorations[decorationCount].type = type;
                    decorations[decorationCount].x = x;
                    decorations[decorationCount].y = y;
                    strcpy(decorations[decorationCount].sector, sectorName);
                    markPositionOccupied(sectorName, x, y, 60.0f);
                    decorationCount++;
                }
            }
        }
        
        printf("[OK] Generated %d decorative elements\n", decorationCount);
    }
    
    // ========== VEHICLE ANIMATION ==========
    
    void initializeVehicleAnimation() {
        // Initialize buses
        for (int i = 0; i < busCount; i++) {
            buses[i].animX = buses[i].x;
            buses[i].animY = buses[i].y;
            buses[i].targetX = buses[i].x;
            buses[i].targetY = buses[i].y;
            buses[i].progress = 1.0f;
            buses[i].pauseTimer = 0.5f + (rand() % 100) / 100.0f;
            buses[i].isMoving = false;
            buses[i].pathLength = 0;
            buses[i].pathIndex = 0;
            // currentStopIdx is now set during loadBusesFromSystem()
            // Only use distance-based fallback if somehow not set
            if (buses[i].currentStopIdx < 0) {
                for (int j = 0; j < stopCount; j++) {
                    float dx = buses[i].x - stops[j].x;
                    float dy = buses[i].y - stops[j].y;
                    if (std::sqrt(dx*dx + dy*dy) < 150.0f) {
                        buses[i].currentStopIdx = j;
                        break;
                    }
                }
            }
        }
        
        // Initialize trains
        for (int i = 0; i < trainCount; i++) {
            trains[i].animX = trains[i].x;
            trains[i].animY = trains[i].y;
            trains[i].targetX = trains[i].x;
            trains[i].targetY = trains[i].y;
            trains[i].progress = 1.0f;
            trains[i].pauseTimer = 0.5f + (rand() % 100) / 100.0f;
            trains[i].isMoving = false;
            trains[i].pathLength = 0;
            trains[i].pathIndex = 0;
            trains[i].currentStationIdx = -1;
            for (int j = 0; j < railwayStationCount; j++) {
                float dx = trains[i].x - railwayStations[j].x;
                float dy = trains[i].y - railwayStations[j].y;
                if (std::sqrt(dx*dx + dy*dy) < 250.0f) {  // Larger radius to find station
                    trains[i].currentStationIdx = j;
                    break;
                }
            }
        }
        
        // Initialize flights
        for (int i = 0; i < flightCount; i++) {
            flights[i].animX = flights[i].x;
            flights[i].animY = flights[i].y;
            flights[i].targetX = flights[i].x;
            flights[i].targetY = flights[i].y;
            flights[i].progress = 1.0f;
            flights[i].pauseTimer = 1.0f + (rand() % 200) / 100.0f;
            flights[i].isMoving = false;
            flights[i].pathLength = 0;
            flights[i].pathIndex = 0;
            flights[i].currentAirportIdx = -1;
            for (int j = 0; j < airportCount; j++) {
                float dx = flights[i].x - airports[j].x;
                float dy = flights[i].y - airports[j].y;
                if (std::sqrt(dx*dx + dy*dy) < 500.0f) {
                    flights[i].currentAirportIdx = j;
                    break;
                }
            }
        }
        
        animationClock.restart();
    }
    
    void pickNextTarget(BusData& bus) {
        // Only pick directly connected stops - no random jumps
        if (bus.currentStopIdx < 0 || connectionCount == 0) return;
        
        // Find all connections from current stop
        int connectedStops[20];
        int connectedCount = 0;
        
        for (int i = 0; i < connectionCount && connectedCount < 20; i++) {
            if (connections[i].fromIdx == bus.currentStopIdx) {
                connectedStops[connectedCount++] = connections[i].toIdx;
            } else if (connections[i].toIdx == bus.currentStopIdx) {
                connectedStops[connectedCount++] = connections[i].fromIdx;
            }
        }
        
        // Pick a random connected stop
        if (connectedCount > 0) {
            int nextStop = connectedStops[rand() % connectedCount];
            bus.targetX = stops[nextStop].x;
            bus.targetY = stops[nextStop].y;
            bus.currentStopIdx = nextStop;  // Update current stop
            bus.isMoving = true;
        }
    }
    
    void updateVehicleAnimation(float deltaTime) {
        if (animationPaused) return;
        
        // Update buses
        for (int i = 0; i < busCount; i++) {
            if (buses[i].pauseTimer > 0) {
                buses[i].pauseTimer -= deltaTime;
                continue;
            }
            
            if (!buses[i].isMoving) {
                pickNextTarget(buses[i]);
            }
            
            if (buses[i].isMoving) {
                float dx = buses[i].targetX - buses[i].animX;
                float dy = buses[i].targetY - buses[i].animY;
                float dist = std::sqrt(dx*dx + dy*dy);
                
                if (dist < 5.0f) {
                    // Reached target
                    buses[i].animX = buses[i].targetX;
                    buses[i].animY = buses[i].targetY;
                    buses[i].isMoving = false;
                    buses[i].pauseTimer = 0.8f + (rand() % 100) / 100.0f;  // Pause at stop
                } else {
                    // Move toward target
                    float moveAmount = vehicleSpeed * deltaTime;
                    buses[i].animX += (dx / dist) * moveAmount;
                    buses[i].animY += (dy / dist) * moveAmount;
                }
            }
        }
        
        // Update trains (using railway connections)
        for (int i = 0; i < trainCount; i++) {
            if (trains[i].pauseTimer > 0) {
                trains[i].pauseTimer -= deltaTime;
                continue;
            }
            
            // Sync station index if lost (e.g. after resize/reposition)
            if (!trains[i].isMoving && trains[i].currentStationIdx == -1) {
                 for (int j = 0; j < railwayStationCount; j++) {
                    float dx = trains[i].x - railwayStations[j].x;
                    float dy = trains[i].y - railwayStations[j].y;
                    if (std::sqrt(dx*dx + dy*dy) < 500.0f) {  // Generous check to re-find station
                        trains[i].currentStationIdx = j;
                        break;
                    }
                }
            }
            
            // Only pick directly connected stations
            if (!trains[i].isMoving && trains[i].currentStationIdx >= 0 && railwayConnectionCount > 0) {
                // Find connected stations
                int connectedStations[10];
                int connectedCount = 0;
                for (int j = 0; j < railwayConnectionCount && connectedCount < 10; j++) {
                    if (railwayConnections[j].fromIdx == trains[i].currentStationIdx) {
                        connectedStations[connectedCount++] = railwayConnections[j].toIdx;
                    } else if (railwayConnections[j].toIdx == trains[i].currentStationIdx) {
                        connectedStations[connectedCount++] = railwayConnections[j].fromIdx;
                    }
                }
                if (connectedCount > 0) {
                    int nextStation = connectedStations[rand() % connectedCount];
                    trains[i].targetX = railwayStations[nextStation].x;
                    trains[i].targetY = railwayStations[nextStation].y;
                    trains[i].currentStationIdx = nextStation;
                    trains[i].isMoving = true;
                }
            }
            
            if (trains[i].isMoving) {
                float dx = trains[i].targetX - trains[i].animX;
                float dy = trains[i].targetY - trains[i].animY;
                float dist = std::sqrt(dx*dx + dy*dy);
                
                if (dist < 5.0f) {
                    trains[i].animX = trains[i].targetX;
                    trains[i].animY = trains[i].targetY;
                    trains[i].isMoving = false;
                    trains[i].pauseTimer = 1.5f + (rand() % 150) / 100.0f;  // Trains pause longer
                } else {
                    float moveAmount = vehicleSpeed * 0.7f * deltaTime;  // Trains a bit slower
                    trains[i].animX += (dx / dist) * moveAmount;
                    trains[i].animY += (dy / dist) * moveAmount;
                }
            }
        }
        
        // Update flights (using airport connections)
        for (int i = 0; i < flightCount; i++) {
            if (flights[i].pauseTimer > 0) {
                flights[i].pauseTimer -= deltaTime;
                continue;
            }
            
            // Only pick directly connected airports
            if (!flights[i].isMoving && flights[i].currentAirportIdx >= 0 && airportConnectionCount > 0) {
                // Find connected airports
                int connectedAirports[10];
                int connectedCount = 0;
                for (int j = 0; j < airportConnectionCount && connectedCount < 10; j++) {
                    if (airportConnections[j].fromIdx == flights[i].currentAirportIdx) {
                        connectedAirports[connectedCount++] = airportConnections[j].toIdx;
                    } else if (airportConnections[j].toIdx == flights[i].currentAirportIdx) {
                        connectedAirports[connectedCount++] = airportConnections[j].fromIdx;
                    }
                }
                if (connectedCount > 0) {
                    int nextAirport = connectedAirports[rand() % connectedCount];
                    flights[i].targetX = airports[nextAirport].x;
                    flights[i].targetY = airports[nextAirport].y;
                    flights[i].currentAirportIdx = nextAirport;
                    flights[i].isMoving = true;
                }
            }
            
            if (flights[i].isMoving) {
                float dx = flights[i].targetX - flights[i].animX;
                float dy = flights[i].targetY - flights[i].animY;
                float dist = std::sqrt(dx*dx + dy*dy);
                
                if (dist < 10.0f) {
                    flights[i].animX = flights[i].targetX;
                    flights[i].animY = flights[i].targetY;
                    flights[i].isMoving = false;
                    flights[i].pauseTimer = 2.0f + (rand() % 200) / 100.0f;  // Flights pause longest
                } else {
                    float moveAmount = vehicleSpeed * 1.5f * deltaTime;  // Planes are fastest
                    flights[i].animX += (dx / dist) * moveAmount;
                    flights[i].animY += (dy / dist) * moveAmount;
                }
            }
        }
    }
    
    // ========== PATHFINDING VISUALIZATION ==========
    
    void enterPathfindingMode(int networkType = 0) {
        pathfindingMode = true;
        pathfindingNetworkType = networkType;  // 0=bus, 1=airport, 2=railway
        pathSourceStop = -1;
        pathDestStop = -1;
        pathStopCount = 0;
        pathDistance = 0.0f;
        pathFound = false;
        // Clear entity selections
        selectedStop = selectedHospital = selectedSchool = -1;
        selectedPharmacy = selectedMall = selectedPark = selectedMosque = -1;
        selectedAirport = selectedRailwayStation = -1;
        selectedBus = selectedTrain = selectedFlight = -1;
    }
    
    void exitPathfindingMode() {
        pathfindingMode = false;
        pathfindingNetworkType = 0;
        pathSourceStop = -1;
        pathDestStop = -1;
        pathStopCount = 0;
        pathDistance = 0.0f;
        pathFound = false;
    }
    
    void clearPath() {
        pathSourceStop = -1;
        pathDestStop = -1;
        pathStopCount = 0;
        pathDistance = 0.0f;
        pathFound = false;
    }
    
    void calculatePath() {
        if (pathSourceStop < 0 || pathDestStop < 0) return;
        
        double distance = 0.0;
        
        if (pathfindingNetworkType == 0 && transport != nullptr) {
            // Bus stops
            const char* sourceId = stops[pathSourceStop].id;
            const char* destId = stops[pathDestStop].id;
            pathStopCount = transport->getVisualPath(sourceId, destId, pathStopIds, distance);
        } else if (pathfindingNetworkType == 1 && airports_sys != nullptr) {
            // Airports
            const char* sourceId = airports[pathSourceStop].id;
            const char* destId = airports[pathDestStop].id;
            pathStopCount = airports_sys->getVisualPath(sourceId, destId, pathStopIds, distance);
        } else if (pathfindingNetworkType == 2 && railway_sys != nullptr) {
            // Railway stations
            const char* sourceId = railwayStations[pathSourceStop].id;
            const char* destId = railwayStations[pathDestStop].id;
            pathStopCount = railway_sys->getVisualPath(sourceId, destId, pathStopIds, distance);
        }
        
        pathDistance = static_cast<float>(distance);
        pathFound = (pathStopCount > 0);
    }
    
    // ========== ENTITY LOADERS ==========
    
    // Load actual stops from TransportSystem
    void loadStopsFromSystem() {
        if (!transport) return;
        stopCount = 0;
        
        // Get all stops from the hashtable
        char** keys = new char*[50];
        int count = 0;
        transport->getStopKeys(keys, &count);
        
        // Count stops per sector for better distribution
        int sectorStopCounts[10] = {0};
        int sectorStartIndices[10] = {0};
        
        for (int i = 0; i < count && stopCount < 50; i++) {
            BusStop* stop = transport->getStop(keys[i]);
            if (stop) {
                strcpy(stops[stopCount].id, stop->stopID);
                strcpy(stops[stopCount].name, stop->name);
                strcpy(stops[stopCount].sector, stop->sector);
                
                // Use collision-free placement
                if (findFreePosition(stop->sector, 35.0f, stops[stopCount].x, stops[stopCount].y)) {
                    markPositionOccupied(stop->sector, stops[stopCount].x, stops[stopCount].y, 35.0f);
                    stopCount++;
                }
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d bus stops from system\n", stopCount);
    }
    
    // Load connections from graph
    void loadConnectionsFromSystem() {
        if (!transport) return;
        connectionCount = 0;
        
        // For each pair of stops, check if connected
        for (int i = 0; i < stopCount; i++) {
            for (int j = i + 1; j < stopCount; j++) {
                double dist = transport->getDistance(stops[i].id, stops[j].id);
                if (dist > 0 && connectionCount < 100) {
                    connections[connectionCount].fromIdx = i;
                    connections[connectionCount].toIdx = j;
                    connections[connectionCount].distance = (float)dist;
                    connectionCount++;
                }
            }
        }
        
        printf("[OK] Loaded %d connections\n", connectionCount);
    }
    
    // Load airport connections from graph
    void loadAirportConnectionsFromSystem() {
        if (!airports_sys) return;
        airportConnectionCount = 0;
        
        // For each pair of airports, check if connected
        for (int i = 0; i < airportCount; i++) {
            for (int j = i + 1; j < airportCount; j++) {
                double dist = airports_sys->getDistance(airports[i].id, airports[j].id);
                if (dist > 0 && airportConnectionCount < 50) {
                    airportConnections[airportConnectionCount].fromIdx = i;
                    airportConnections[airportConnectionCount].toIdx = j;
                    airportConnections[airportConnectionCount].distance = (float)dist;
                    airportConnectionCount++;
                }
            }
        }
        
        printf("[OK] Loaded %d airport connections\n", airportConnectionCount);
    }
    
    // Load railway connections from graph
    void loadRailwayConnectionsFromSystem() {
        if (!railway_sys) return;
        railwayConnectionCount = 0;
        
        // For each pair of stations, check if connected
        for (int i = 0; i < railwayStationCount; i++) {
            for (int j = i + 1; j < railwayStationCount; j++) {
                double dist = railway_sys->getDistance(railwayStations[i].id, railwayStations[j].id);
                if (dist > 0 && railwayConnectionCount < 50) {
                    railwayConnections[railwayConnectionCount].fromIdx = i;
                    railwayConnections[railwayConnectionCount].toIdx = j;
                    railwayConnections[railwayConnectionCount].distance = (float)dist;
                    railwayConnectionCount++;
                }
            }
        }
        
        printf("[OK] Loaded %d railway connections\n", railwayConnectionCount);
    }
    
    // Load hospitals from MedicalSystem
    void loadHospitalsFromSystem() {
        if (!medical) return;
        hospitalCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        medical->getHospitalKeys(keys, &count);
        
        for (int i = 0; i < count && hospitalCount < 20; i++) {
            Hospital* hosp = medical->getHospital(keys[i]);
            if (hosp) {
                strcpy(hospitals[hospitalCount].id, hosp->hospitalID);
                strcpy(hospitals[hospitalCount].name, hosp->name);
                strcpy(hospitals[hospitalCount].sector, hosp->sector);
                
                // Use collision-free placement (60px radius for hospitals)
                if (!findFreePosition(hosp->sector, 60.0f, hospitals[hospitalCount].x, hospitals[hospitalCount].y)) {
                    cityMap.getSectorPosition(hosp->sector, hospitals[hospitalCount].x, hospitals[hospitalCount].y);
                }
                markPositionOccupied(hosp->sector, hospitals[hospitalCount].x, hospitals[hospitalCount].y, 60.0f);
                hospitalCount++;
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d hospitals\n", hospitalCount);
    }
    
    // Load schools from EducationSystem
    void loadSchoolsFromSystem() {
        if (!education) return;
        schoolCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        education->getSchoolKeys(keys, &count);
        
        for (int i = 0; i < count && schoolCount < 20; i++) {
            School* school = education->getSchool(keys[i]);
            if (school) {
                strcpy(schools[schoolCount].id, school->schoolID);
                strcpy(schools[schoolCount].name, school->name);
                strcpy(schools[schoolCount].sector, school->sector);
                
                // Use collision-free placement (50px radius for schools)
                if (!findFreePosition(school->sector, 50.0f, schools[schoolCount].x, schools[schoolCount].y)) {
                    cityMap.getSectorPosition(school->sector, schools[schoolCount].x, schools[schoolCount].y);
                }
                markPositionOccupied(school->sector, schools[schoolCount].x, schools[schoolCount].y, 50.0f);
                schoolCount++;
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d schools\n", schoolCount);
    }
    
    // Load pharmacies from MedicalSystem
    void loadPharmaciesFromSystem() {
        if (!medical) return;
        pharmacyCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        medical->getPharmacyKeys(keys, &count);
        
        for (int i = 0; i < count && pharmacyCount < 20; i++) {
            Pharmacy* pharm = medical->getPharmacy(keys[i]);
            if (pharm) {
                strcpy(pharmacies[pharmacyCount].id, pharm->pharmacyID);
                strcpy(pharmacies[pharmacyCount].name, pharm->name);
                strcpy(pharmacies[pharmacyCount].sector, pharm->sector);
                
                // Use collision-free placement (50px radius for pharmacies)
                if (!findFreePosition(pharm->sector, 50.0f, pharmacies[pharmacyCount].x, pharmacies[pharmacyCount].y)) {
                     cityMap.getSectorPosition(pharm->sector, pharmacies[pharmacyCount].x, pharmacies[pharmacyCount].y);
                }
                markPositionOccupied(pharm->sector, pharmacies[pharmacyCount].x, pharmacies[pharmacyCount].y, 50.0f);
                pharmacyCount++;
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d pharmacies\n", pharmacyCount);
    }
    
    // Load malls from CommercialSystem
    void loadMallsFromSystem() {
        if (!commercial) return;
        mallCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        commercial->getMallKeys(keys, &count);
        
        for (int i = 0; i < count && mallCount < 20; i++) {
            Mall* mall = commercial->getMall(keys[i]);
            if (mall) {
                strcpy(malls[mallCount].id, mall->mallID);
                strcpy(malls[mallCount].name, mall->name);
                strcpy(malls[mallCount].sector, mall->sector);
                
                // Use collision-free placement (60px radius for malls)
                if (!findFreePosition(mall->sector, 60.0f, malls[mallCount].x, malls[mallCount].y)) {
                    cityMap.getSectorPosition(mall->sector, malls[mallCount].x, malls[mallCount].y);
                }
                markPositionOccupied(mall->sector, malls[mallCount].x, malls[mallCount].y, 60.0f);
                mallCount++;
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d malls\n", mallCount);
    }
    
    // Load parks from PublicFacilitySystem
    void loadParksFromSystem() {
        if (!publicFacilities) return;
        parkCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        publicFacilities->getParkKeys(keys, &count);
        
        for (int i = 0; i < count && parkCount < 20; i++) {
            Facility* park = publicFacilities->getPark(keys[i]);
            if (park) {
                strcpy(parks[parkCount].id, park->facilityID);
                strcpy(parks[parkCount].name, park->name);
                strcpy(parks[parkCount].sector, park->sector);
                
                // Use collision-free placement (90px radius for parks - reduced to fit)
                if (!findFreePosition(park->sector, 90.0f, parks[parkCount].x, parks[parkCount].y)) {
                    // Fallback to sector position so it's in the list
                    cityMap.getSectorPosition(park->sector, parks[parkCount].x, parks[parkCount].y);
                }
                markPositionOccupied(park->sector, parks[parkCount].x, parks[parkCount].y, 90.0f);
                parkCount++;
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d parks\n", parkCount);
    }
    
    // Load mosques from PublicFacilitySystem
    void loadMosquesFromSystem() {
        if (!publicFacilities) return;
        mosqueCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        publicFacilities->getMosqueKeys(keys, &count);
        
        for (int i = 0; i < count && mosqueCount < 20; i++) {
            Facility* mosque = publicFacilities->getMosque(keys[i]);
            if (mosque) {
                strcpy(mosques[mosqueCount].id, mosque->facilityID);
                strcpy(mosques[mosqueCount].name, mosque->name);
                strcpy(mosques[mosqueCount].sector, mosque->sector);
                
                // Use collision-free placement (40px radius for mosques - reduced)
                if (!findFreePosition(mosque->sector, 40.0f, mosques[mosqueCount].x, mosques[mosqueCount].y)) {
                    cityMap.getSectorPosition(mosque->sector, mosques[mosqueCount].x, mosques[mosqueCount].y);
                }
                markPositionOccupied(mosque->sector, mosques[mosqueCount].x, mosques[mosqueCount].y, 40.0f);
                mosqueCount++;
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d mosques\n", mosqueCount);
    }
    
    // Load airports from AirportSystem
    void loadAirportsFromSystem() {
        if (!airports_sys) return;
        airportCount = 0;
        
        char** keys = new char*[10];
        int count = 0;
        airports_sys->getAirportKeys(keys, &count);
        
        for (int i = 0; i < count && airportCount < 10; i++) {
            Airport* airport = airports_sys->getAirport(keys[i]);
            if (airport) {
                strcpy(airports[airportCount].id, airport->code);
                strcpy(airports[airportCount].name, airport->name);
                strcpy(airports[airportCount].sector, airport->sector);
                
                // Use collision-free placement (80px radius for airports - reduced)
                if (!findFreePosition(airport->sector, 80.0f, airports[airportCount].x, airports[airportCount].y)) {
                    cityMap.getSectorPosition(airport->sector, airports[airportCount].x, airports[airportCount].y);
                }
                markPositionOccupied(airport->sector, airports[airportCount].x, airports[airportCount].y, 80.0f);
                airportCount++;
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d airports\n", airportCount);
        for (int i = 0; i < airportCount; i++) {
            printf("  Airport %s at (%.1f, %.1f) sector=%s\n", airports[i].id, airports[i].x, airports[i].y, airports[i].sector);
        }
    }
    
    // Load railway stations from RailwaySystem
    void loadRailwayStationsFromSystem() {
        if (!railway_sys) return;
        railwayStationCount = 0;
        
        char** keys = new char*[10];
        int count = 0;
        railway_sys->getStationKeys(keys, &count);
        
        for (int i = 0; i < count && railwayStationCount < 10; i++) {
            RailStation* station = railway_sys->getStation(keys[i]);
            if (station) {
                strcpy(railwayStations[railwayStationCount].id, station->stationID);
                strcpy(railwayStations[railwayStationCount].name, station->name);
                strcpy(railwayStations[railwayStationCount].sector, station->sector);
                
                // Use collision-free placement (70px radius for railway stations - reduced)
                if (!findFreePosition(station->sector, 70.0f, railwayStations[railwayStationCount].x, railwayStations[railwayStationCount].y)) {
                    cityMap.getSectorPosition(station->sector, railwayStations[railwayStationCount].x, railwayStations[railwayStationCount].y);
                }
                markPositionOccupied(station->sector, railwayStations[railwayStationCount].x, railwayStations[railwayStationCount].y, 70.0f);
                railwayStationCount++;
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d railway stations\n", railwayStationCount);
        for (int i = 0; i < railwayStationCount; i++) {
            printf("  Railway %s at (%.1f, %.1f) sector=%s\n", railwayStations[i].id, railwayStations[i].x, railwayStations[i].y, railwayStations[i].sector);
        }
    }
    
    // Load buses from TransportSystem
    void loadBusesFromSystem() {
        if (!transport) return;
        busCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        transport->getBusKeys(keys, &count);
        
        for (int i = 0; i < count && busCount < 10; i++) {
            Bus* bus = transport->getBus(keys[i]);
            if (bus) {
                strcpy(buses[busCount].id, bus->busNo);
                strcpy(buses[busCount].name, bus->company);
                strcpy(buses[busCount].currentStop, bus->currentStop);
                
                // Position bus at its current stop (with offset)
                bool positioned = false;
                for (int j = 0; j < stopCount; j++) {
                    if (strcmp(stops[j].id, bus->currentStop) == 0) {
                        // Use collision-free placement (40px radius for buses)
                        if (findFreePosition(stops[j].sector, 40.0f, buses[busCount].x, buses[busCount].y)) {
                            markPositionOccupied(stops[j].sector, buses[busCount].x, buses[busCount].y, 40.0f);
                            // Store stop index directly for animation (don't rely on distance check)
                            buses[busCount].currentStopIdx = j;
                            busCount++;
                            positioned = true;
                        }
                        break;
                    }
                }
                
                // Fallback if stop not found or placement failed
                if (!positioned) {
                    buses[busCount].x = 100;
                    buses[busCount].y = 100;
                    buses[busCount].currentStopIdx = -1;  // No valid stop
                    // If fallback, still increment busCount if not already incremented by findFreePosition
                    if (busCount < 10) { // Ensure we don't exceed array bounds
                        busCount++;
                    }
                }
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d buses\n", busCount);
    }
    
    // Load trains from RailwaySystem
    void loadTrainsFromSystem() {
        if (!railway_sys) return;
        trainCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        railway_sys->getTrainKeys(keys, &count);
        
        for (int i = 0; i < count && trainCount < 20; i++) {
            Train* train = railway_sys->getTrain(keys[i]);
            if (train) {
                strcpy(trains[trainCount].id, train->trainNo);
                strcpy(trains[trainCount].name, train->name);
                strcpy(trains[trainCount].currentStation, train->currentLocation);
                
                // Position near current station with collision-free placement
                for (int j = 0; j < railwayStationCount; j++) {
                    if (strcmp(railwayStations[j].id, train->currentLocation) == 0) {
                        // Use collision-free placement (40px radius for trains)
                        if (findFreePosition(railwayStations[j].sector, 40.0f, trains[trainCount].x, trains[trainCount].y)) {
                            markPositionOccupied(railwayStations[j].sector, trains[trainCount].x, trains[trainCount].y, 40.0f);
                            trainCount++;
                        }
                        break;
                    }
                }
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d trains\n", trainCount);
    }
    
    // Load flights from AirportSystem
    void loadFlightsFromSystem() {
        if (!airports_sys) return;
        flightCount = 0;
        
        char** keys = new char*[20];
        int count = 0;
        airports_sys->getFlightKeys(keys, &count);
        
        for (int i = 0; i < count && flightCount < 20; i++) {
            Flight* flight = airports_sys->getFlight(keys[i]);
            if (flight) {
                strcpy(flights[flightCount].id, flight->flightNo);
                strcpy(flights[flightCount].airline, flight->airline);
                strcpy(flights[flightCount].currentAirport, flight->currentLocation);
                
                // Position at current airport with collision-free placement
                for (int j = 0; j < airportCount; j++) {
                    if (strcmp(airports[j].id, flight->currentLocation) == 0) {
                        // Use collision-free placement (35px radius for flights)
                        if (findFreePosition(airports[j].sector, 35.0f, flights[flightCount].x, flights[flightCount].y)) {
                            markPositionOccupied(airports[j].sector, flights[flightCount].x, flights[flightCount].y, 35.0f);
                            flightCount++;
                        }
                        break;
                    }
                }
            }
        }
        
        delete[] keys;
        printf("[OK] Loaded %d flights\n", flightCount);
    }
    
    // Reload ALL data from systems (for dynamic updates after CLI changes)
    void reloadAllData(PopulationSystem* pop, TransportSystem* trans, 
                      MedicalSystem* med, EducationSystem* edu,
                      CommercialSystem* comm, PublicFacilitySystem* pub,
                      AirportSystem* air, RailwaySystem* rail) {
        printf("\n[RELOAD] Refreshing visualization data...\n");
        
        // Reset window if it was closed
        if (!window.isOpen()) {
            window.create(sf::VideoMode({1920u, 1080u}), "ISBGlyph - Islamabad");
            window.setFramerateLimit(60);
        }
        running = true;  // Reset running flag
        
        // Clear spatial grids for fresh placement
        clearSectorGrids();
        
        // Clear ALL entity data
        stopCount = 0;
        hospitalCount = 0;
        schoolCount = 0;
        pharmacyCount = 0;
        mallCount = 0;
        parkCount = 0;
        airportCount = 0;
        railwayStationCount = 0;
        connectionCount = 0;
        busCount = 0;
        trainCount = 0;
        flightCount = 0;
        mosqueCount = 0;
        selectedStop = -1;
        selectedHospital = -1;
        selectedSchool = -1;
        
        // Update system pointers
        commercial = comm;
        publicFacilities = pub;
        airports_sys = air;
        railway_sys = rail;
        
        // Re-collect sectors from ALL systems
        collectAllSectors(pop, trans, med, edu);
        
        // Reload ALL entities
        if (trans) {
            loadStopsFromSystem();
            loadConnectionsFromSystem();
            loadBusesFromSystem();
        }
        if (med) {
            loadHospitalsFromSystem();
            loadPharmaciesFromSystem();
        }
        if (edu) {
            loadSchoolsFromSystem();
        }
        if (air) {
            loadAirportsFromSystem();
            loadFlightsFromSystem();
            loadAirportConnectionsFromSystem();
        }
        if (rail) {
            loadRailwayStationsFromSystem();
            loadTrainsFromSystem();
            loadRailwayConnectionsFromSystem();
        }
        if (comm) {
            loadMallsFromSystem();
        }
        if (pub) {
            loadParksFromSystem();
            loadMosquesFromSystem();
        }
        
        printf("[RELOAD] Complete! %d stops, %d hospitals, %d schools, %d pharmacies, %d malls, %d parks, %d mosques, %d airports, %d stations, %d buses, %d trains, %d flights\n", 
               stopCount, hospitalCount, schoolCount, pharmacyCount, mallCount, parkCount, mosqueCount, airportCount, railwayStationCount, busCount, trainCount, flightCount);
        
        // ===== DYNAMIC SECTOR SIZING =====
        // Calculate sector sizes based on entity count BEFORE generating decorations
        calculateSectorSizes();
        cityMap.recalculateGridPositions();
        
        // Entities need to be repositioned after grid recalculation
        repositionAllEntities();
        
        // Generate decorative elements for all sectors
        generateSectorDecorations();
        
        // Load and Spawn Animated People
        loadPeopleTextures();
        spawnPeople();
        
        // Initialize vehicle animation after loading
        initializeVehicleAnimation();
    }
    
    // Calculate dynamic sector sizes based on entity count
    void calculateSectorSizes() {
        // Use sectorGrids to count entities per sector
        for (int i = 0; i < sectorGridCount; i++) {
            cityMap.setSectorEntityCount(sectorGrids[i].sectorName, sectorGrids[i].count);
        }
        printf("[OK] Calculated sector sizes based on entity counts\n");
    }
    
    // Reposition all entities after grid recalculation
    void repositionAllEntities() {
        bool needsResizing = false;
        int resizeAttempts = 0;
        const int MAX_RESIZE_ATTEMPTS = 5; // Prevent infinite loops

        do {
            needsResizing = false;
            clearSectorGrids(); // Clear grid to restart placement

            // Helper lambda to place bad items safely
            // Returns true if placement succeeded, false if resizing is needed
            auto placeEntity = [&](const char* sector, float radius, float* x, float* y) -> bool {
                if (findFreePosition(sector, radius, *x, *y)) {
                     markPositionOccupied(sector, *x, *y, radius);
                     return true;
                } else {
                    // Placement failed!
                    // If we haven't hit max retries, request expansion
                    if (resizeAttempts < MAX_RESIZE_ATTEMPTS) {
                        printf("[INFO] Expansion trigger for %s due to fit failure.\n", sector);
                        cityMap.expandSectorBounds(sector, 200.0f); // Add chunk of space
                        cityMap.recalculateGridPositions(); // Recalc grid layout
                        return false; // Signal to restart loop
                    } else {
                        // Max retries hit, fallback to "Stay Put" to prevent vanishing
                        // (Ideally we found space by now, but this is the ultimate safety net)
                        markPositionOccupied(sector, *x, *y, radius);
                        return true;
                    }
                }
            };

            // Re-place stops
            for (int i = 0; i < stopCount; i++) {
                if (!placeEntity(stops[i].sector, 60.0f, &stops[i].x, &stops[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }

            // Re-place hospitals
            for (int i = 0; i < hospitalCount; i++) {
                if (!placeEntity(hospitals[i].sector, 60.0f, &hospitals[i].x, &hospitals[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }

            // Re-place schools
            for (int i = 0; i < schoolCount; i++) {
                if (!placeEntity(schools[i].sector, 50.0f, &schools[i].x, &schools[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }

            // Re-place pharmacies
            for (int i = 0; i < pharmacyCount; i++) {
                if (!placeEntity(pharmacies[i].sector, 50.0f, &pharmacies[i].x, &pharmacies[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }

            // Re-place malls
            for (int i = 0; i < mallCount; i++) {
                if (!placeEntity(malls[i].sector, 60.0f, &malls[i].x, &malls[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }

            // Re-place parks
            for (int i = 0; i < parkCount; i++) {
                if (!placeEntity(parks[i].sector, 90.0f, &parks[i].x, &parks[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }

            // Re-place mosques
            for (int i = 0; i < mosqueCount; i++) {
                if (!placeEntity(mosques[i].sector, 40.0f, &mosques[i].x, &mosques[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }

            // Re-place airports
            for (int i = 0; i < airportCount; i++) {
                if (!placeEntity(airports[i].sector, 80.0f, &airports[i].x, &airports[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }

            // Re-place railway stations
            for (int i = 0; i < railwayStationCount; i++) {
                if (!placeEntity(railwayStations[i].sector, 60.0f, &railwayStations[i].x, &railwayStations[i].y)) {
                    needsResizing = true; break;
                }
            }
            if (needsResizing) { resizeAttempts++; continue; }
            
            // Vehicles don't trigger expansion, they just go to their stations/stops
             // Re-place buses at their current stops
            for (int i = 0; i < busCount; i++) {
                for (int j = 0; j < stopCount; j++) {
                    if (strcmp(stops[j].id, buses[i].currentStop) == 0) {
                        if (findFreePosition(stops[j].sector, 40.0f, buses[i].x, buses[i].y)) {
                            markPositionOccupied(stops[j].sector, buses[i].x, buses[i].y, 40.0f);
                            buses[i].currentStopIdx = j;
                        } else {
                            // Vehicles just force-place, they don't expand sectors
                             markPositionOccupied(stops[j].sector, buses[i].x, buses[i].y, 40.0f);
                        }
                        break;
                    }
                }
            }
            
            // Re-place trains...
            for (int i = 0; i < trainCount; i++) {
                for (int j = 0; j < railwayStationCount; j++) {
                    if (strcmp(railwayStations[j].id, trains[i].currentStation) == 0) {
                        if (findFreePosition(railwayStations[j].sector, 50.0f, trains[i].x, trains[i].y)) {
                            markPositionOccupied(railwayStations[j].sector, trains[i].x, trains[i].y, 50.0f);
                            trains[i].currentStationIdx = j;
                        } else {
                            markPositionOccupied(railwayStations[j].sector, trains[i].x, trains[i].y, 50.0f);
                        }
                        break;
                    }
                }
            }
            
            // Re-place flights...
            for (int i = 0; i < flightCount; i++) {
                for (int j = 0; j < airportCount; j++) {
                    if (strcmp(airports[j].id, flights[i].currentAirport) == 0) {
                        if (findFreePosition(airports[j].sector, 35.0f, flights[i].x, flights[i].y)) {
                            markPositionOccupied(airports[j].sector, flights[i].x, flights[i].y, 35.0f);
                            flights[i].currentAirportIdx = j;
                            flights[i].animX = flights[i].x;
                            flights[i].animY = flights[i].y;
                            flights[i].targetX = flights[i].x;
                            flights[i].targetY = flights[i].y;
                            flights[i].progress = 1.0f;
                            flights[i].isMoving = false;
                        } else {
                            markPositionOccupied(airports[j].sector, flights[i].x, flights[i].y, 35.0f);
                        }
                        break;
                    }
                }
            }

        } while (needsResizing && resizeAttempts <= MAX_RESIZE_ATTEMPTS);
        
        printf("[OK] Repositioned all entities. Resize attempts: %d\n", resizeAttempts);
    }
    
    void handleEvents() {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                running = false;
                window.close();
            }
            
            // ===== SEARCH INPUT HANDLING =====
            if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                // / key - Focus search box
                if (key->scancode == sf::Keyboard::Scancode::Slash && !searchBoxFocused) {
                    searchBoxFocused = true;
                    searchQuery[0] = '\0';
                    searchResultCount = 0;
                    continue;
                }
                
                // Escape - Clear search or close app
                if (key->scancode == sf::Keyboard::Scancode::Escape) {
                    if (searchBoxFocused) {
                        searchBoxFocused = false;
                        searchQuery[0] = '\0';
                        searchResultCount = 0;
                        selectedSearchResult = -1;
                    } else {
                        running = false;
                        window.close();
                    }
                    continue;
                }
                
                // If search is focused, handle search-specific keys
                if (searchBoxFocused) {
                    // Enter - Select current result
                    if (key->scancode == sf::Keyboard::Scancode::Enter) {
                        if (selectedSearchResult >= 0 && selectedSearchResult < searchResultCount) {
                            selectSearchResult(selectedSearchResult);
                        } else if (searchResultCount > 0) {
                            selectSearchResult(0);  // Select first result
                        }
                        continue;
                    }
                    
                    // Up/Down - Navigate results
                    if (key->scancode == sf::Keyboard::Scancode::Up) {
                        if (selectedSearchResult > 0) {
                            selectedSearchResult--;
                        } else {
                            selectedSearchResult = std::min(searchResultCount - 1, 2);  // Wrap to last visible (max 3)
                        }
                        continue;
                    }
                    if (key->scancode == sf::Keyboard::Scancode::Down) {
                        if (selectedSearchResult < std::min(searchResultCount - 1, 2)) {
                            selectedSearchResult++;
                        } else {
                            selectedSearchResult = 0;  // Wrap to first
                        }
                        continue;
                    }
                    
                    // Backspace - Delete character
                    if (key->scancode == sf::Keyboard::Scancode::Backspace) {
                        int len = strlen(searchQuery);
                        if (len > 0) {
                            searchQuery[len - 1] = '\0';
                            updateSearch();
                        }
                        continue;
                    }
                }
                
                // Don't process toggle keys if search is focused
                if (searchBoxFocused) continue;
            }
            
            // Text input for search
            if (auto* text = event->getIf<sf::Event::TextEntered>()) {
                if (searchBoxFocused) {
                    char c = static_cast<char>(text->unicode);
                    // Only accept printable characters
                    if (c >= 32 && c < 127 && c != '/' && strlen(searchQuery) < 99) {
                        int len = strlen(searchQuery);
                        searchQuery[len] = c;
                        searchQuery[len + 1] = '\0';
                        updateSearch();
                    }
                }
            }
            
            if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                // Connection visibility cycle: 1 key cycles through all connection types
                // State 0: Bus OFF, 1: Airport OFF, 2: Railway OFF, 3: Bus ON, 4: Airport ON, 5: Railway ON
                if (key->scancode == sf::Keyboard::Scancode::Num1) {
                    if (connectionCycleState == 0) {
                        showConnections = false;  // Turn OFF bus connections
                    } else if (connectionCycleState == 1) {
                        showAirportConnections = false;  // Turn OFF airport connections
                    } else if (connectionCycleState == 2) {
                        showRailwayConnections = false;  // Turn OFF railway connections
                    } else if (connectionCycleState == 3) {
                        showConnections = true;  // Turn ON bus connections
                    } else if (connectionCycleState == 4) {
                        showAirportConnections = true;  // Turn ON airport connections
                    } else if (connectionCycleState == 5) {
                        showRailwayConnections = true;  // Turn ON railway connections
                    }
                    connectionCycleState = (connectionCycleState + 1) % 6;
                }
                if (key->scancode == sf::Keyboard::Scancode::Num2) showHospitals = !showHospitals;
                if (key->scancode == sf::Keyboard::Scancode::Num3) showSchools = !showSchools;
                if (key->scancode == sf::Keyboard::Scancode::Num4) showPharmacies = !showPharmacies;
                if (key->scancode == sf::Keyboard::Scancode::Num5) showMalls = !showMalls;
                if (key->scancode == sf::Keyboard::Scancode::Num6) showParks = !showParks;
                if (key->scancode == sf::Keyboard::Scancode::Num7) showAirports = !showAirports;
                if (key->scancode == sf::Keyboard::Scancode::Num8) showRailways = !showRailways;
                if (key->scancode == sf::Keyboard::Scancode::Num9) showBuses = !showBuses;
                if (key->scancode == sf::Keyboard::Scancode::Num0) showTrains = !showTrains;
                if (key->scancode == sf::Keyboard::Scancode::Hyphen) showFlights = !showFlights;
                if (key->scancode == sf::Keyboard::Scancode::Equal) showMosques = !showMosques;
                if (key->scancode == sf::Keyboard::Scancode::Semicolon) showDecorations = !showDecorations;
                if (key->scancode == sf::Keyboard::Scancode::I) showInstructions = !showInstructions;
                
                // Pathfinding mode toggle - cycles: Off → Buses → Airports → Railways → Off
                if (key->scancode == sf::Keyboard::Scancode::P) {
                    if (!pathfindingMode) {
                        enterPathfindingMode(0);  // Start with buses
                    } else if (pathfindingNetworkType == 0) {
                        clearPath();
                        enterPathfindingMode(1);  // Switch to airports
                    } else if (pathfindingNetworkType == 1) {
                        clearPath();
                        enterPathfindingMode(2);  // Switch to railways
                    } else {
                        exitPathfindingMode();    // Exit mode
                    }
                }
                
                // Animation pause toggle
                if (key->scancode == sf::Keyboard::Scancode::Space) {
                    animationPaused = !animationPaused;
                }
                
                // Toggle People Visibility
                if (key->scancode == sf::Keyboard::Scancode::Semicolon) {
                    showPeople = !showPeople;
                    printf("[INFO] Toggled People: %s\n", showPeople ? "ON" : "OFF");
                }
                
                // Reset camera and vehicles
                if (key->scancode == sf::Keyboard::Scancode::R) {
                    camX = 0;
                    camY = 0;
                    zoom = 1.0f;
                    // Clear all entity selections
                    selectedStop = selectedHospital = selectedSchool = -1;
                    selectedPharmacy = selectedMall = selectedPark = selectedMosque = -1;
                    selectedAirport = selectedRailwayStation = -1;
                    selectedBus = selectedTrain = selectedFlight = -1;
                    // Reset vehicle animations
                    initializeVehicleAnimation();
                    animationPaused = true;  // Pause after reset
                }
            }
            
            // Mouse wheel zoom
            if (auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                zoom += wheel->delta * 0.1f;
                if (zoom < 0.3f) zoom = 0.3f;
                if (zoom > 3.5f) zoom = 3.5f;  // Max 350%
            }
            
            // Click detection
            if (auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    // Check if clicking on search results dropdown
                    if (searchBoxFocused && searchResultCount > 0) {
                        float searchY = 55;
                        float dropdownY = searchY + 40;
                        int maxResults = std::min(searchResultCount, 3);
                        
                        if (click->position.x >= 15 && click->position.x <= (15 + UI_WIDTH - 30) &&
                            click->position.y >= dropdownY && click->position.y <= (dropdownY + maxResults * 30.0f)) {
                            // Calculate which result was clicked
                            int resultIndex = static_cast<int>((click->position.y - dropdownY) / 30.0f);
                            if (resultIndex >= 0 && resultIndex < maxResults) {
                                selectSearchResult(resultIndex);
                            }
                            continue;  // Don't propagate to handleClick
                        }
                    }
                    
                    handleClick(click->position.x, click->position.y);
                }
            }
        }
    }

    // Load textures for people
    void loadPeopleTextures() {
        if (!texPersonM.loadFromFile("assets/sprites/T_person.png")) printf("[WARN] T_person.png missing\n");
        if (!texPersonF.loadFromFile("assets/sprites/T_personFemale.png")) printf("[WARN] T_personFemale.png missing\n");
        if (!texPolice.loadFromFile("assets/sprites/T_policeman.png")) printf("[WARN] T_policeman.png missing\n");
        if (!texDoctor.loadFromFile("assets/sprites/T_doctor.png")) printf("[WARN] T_doctor.png missing\n");
        if (!texStudent.loadFromFile("assets/sprites/T_student.png")) printf("[WARN] T_student.png missing\n");
        if (!texEngineer.loadFromFile("assets/sprites/T_engineer.png")) printf("[WARN] T_engineer.png missing\n");
        
        // Use smooth scaling
        texPersonM.setSmooth(true); texPersonF.setSmooth(true);
        texPolice.setSmooth(true); texDoctor.setSmooth(true);
        texStudent.setSmooth(true); texEngineer.setSmooth(true);
    }

    // Spawn animated people based on city context
    void spawnPeople() {
        personCount = 0;
        
        // Helper to spawn a context-aware person (Police, Doctor, etc.)
        auto spawnContextPerson = [&](int type, float cx, float cy) {
            if (personCount >= 500) return;
            people[personCount].type = type;
            people[personCount].baseX = cx;
            people[personCount].baseY = cy;
            // Spawn random distance from center
            float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
            float dist = 80.0f + (rand() % 40);
            people[personCount].x = cx + cos(angle) * dist;
            people[personCount].y = cy + sin(angle) * dist;
            people[personCount].targetX = cx; // Initially target the building
            people[personCount].targetY = cy;
            people[personCount].state = (rand() % 2); // 0=Roaming, 1=Entering
            if (people[personCount].state == 0) {
                 // Roaming: pick random target around base
                 float wa = (float)(rand() % 360) * 3.14159f / 180.0f;
                 float wd = 60.0f + (rand() % 60);
                 people[personCount].targetX = cx + cos(wa) * wd;
                 people[personCount].targetY = cy + sin(wa) * wd;
            }
            people[personCount].speed = 15.0f + (rand() % 10);
            people[personCount].opacity = 255.0f;
            people[personCount].waitTimer = 0.0f;
            personCount++;
        };

        // 1. Police at Police Stations (Deco Type 6)
        for (int i = 0; i < decorationCount; i++) {
            if (decorations[i].type == 6) { // Police Station
                for (int p = 0; p < 4; p++) spawnContextPerson(2, decorations[i].x, decorations[i].y);
            }
            // 2. Engineers at Factories (Deco Type 2)
            if (decorations[i].type == 2) { // Factory
                for (int p = 0; p < 4; p++) spawnContextPerson(5, decorations[i].x, decorations[i].y);
            }
        }

        // 3. Doctors at Hospitals
        for (int i = 0; i < hospitalCount; i++) {
            for (int p = 0; p < 4; p++) spawnContextPerson(3, hospitals[i].x, hospitals[i].y);
        }

        // 4. Students at Schools
        for (int i = 0; i < schoolCount; i++) {
             for (int p = 0; p < 4; p++) spawnContextPerson(4, schools[i].x, schools[i].y);
        }

        // 5. Citizens roaming per sector
        for (int i = 0; i < cityMap.getSectorCount(); i++) {
            Sector* sector = cityMap.getSector(i);
            for (int p = 0; p < 4; p++) { // 3-4 per sector
                if (personCount >= 500) break;
                people[personCount].type = (rand() % 2); // 0=Male, 1=Female
                people[personCount].baseX = sector->centerX; // Use sector center as anchor
                people[personCount].baseY = sector->centerY;
                
                // Random position within sector bounds (approx safe area)
                float w = sector->width * 0.4f; 
                float h = sector->height * 0.4f;
                people[personCount].x = sector->centerX + (rand() % (int)w) - w/2;
                people[personCount].y = sector->centerY + (rand() % (int)h) - h/2;
                
                people[personCount].targetX = sector->centerX + (rand() % (int)w) - w/2;
                people[personCount].targetY = sector->centerY + (rand() % (int)h) - h/2;
                
                people[personCount].state = 0; // Always roaming
                people[personCount].speed = 10.0f + (rand() % 10);
                people[personCount].opacity = 255.0f;
                personCount++;
            }
        }
        printf("[OK] Spawned %d animated people\n", personCount);
    }
    
    // Update people animation
    void updatePeopleAnimation(float dt) {
        if (animationPaused) return;

        for (int i = 0; i < personCount; i++) {
            PersonData& p = people[i];
            
            // Move towards target
            float dx = p.targetX - p.x;
            float dy = p.targetY - p.y;
            float dist = sqrt(dx*dx + dy*dy);
            
            if (dist > 5.0f) {
                float move = p.speed * dt * 5.0f; // Speed multiplier
                p.x += (dx / dist) * move;
                p.y += (dy / dist) * move;
            } else {
                // Reached target
                if (p.type >= 2) { // Professoinals (Context Aware)
                    if (p.state == 1) { // Was Entering
                        p.opacity -= 10.0f * dt * 10.0f; // Fade out
                        if (p.opacity <= 0) {
                            p.opacity = 0;
                            p.waitTimer = 1.0f + (rand() % 2); // Wait inside
                            p.state = 3; // Waiting inside
                        }
                    } else if (p.state == 3) { // Waiting Inside
                        p.waitTimer -= dt;
                        if (p.waitTimer <= 0) {
                            // Teleport to edge and Exit
                            float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                            p.x = p.baseX + cos(angle) * 20.0f; // Start closer
                            p.y = p.baseY + sin(angle) * 20.0f;
                            p.targetX = p.baseX + cos(angle) * 120.0f; // Walk out
                            p.targetY = p.baseY + sin(angle) * 120.0f;
                            p.state = 2; // Exiting
                            p.opacity = 0;
                        }
                    } else if (p.state == 2) { // Was Exiting
                         p.opacity += 10.0f * dt * 10.0f; // Fade in
                         if (p.opacity >= 255) {
                             p.opacity = 255;
                             // Switch to Entering
                             p.targetX = p.baseX;
                             p.targetY = p.baseY;
                             p.state = 1;
                         }
                    } else { // Roaming/Patrolling
                         // Pick new random patrol point
                         float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                         float range = 100.0f + (rand() % 50);
                         p.targetX = p.baseX + cos(angle) * range;
                         p.targetY = p.baseY + sin(angle) * range;
                         // Occasional switch to Entering (20% chance)
                         if ((rand() % 100) < 20) {
                             p.targetX = p.baseX;
                             p.targetY = p.baseY;
                             p.state = 1; 
                         }
                    }
                } else { // Citizens (Roaming)
                    // Pick new random point in sector bounds
                    float dx = (rand() % 400) - 200.0f;
                    float dy = (rand() % 300) - 150.0f;
                    p.targetX = p.baseX + dx;
                    p.targetY = p.baseY + dy;
                }
            }
        }
    }
    void update() {
        // Update vehicle animation
        float deltaTime = animationClock.restart().asSeconds();
        updateVehicleAnimation(deltaTime);
        updatePeopleAnimation(deltaTime);
        
        // Camera movement with WASD
        float moveSpeed = 25.0f / zoom;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) camY += moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) camY -= moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) camX += moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) camX -= moveSpeed;
        
        // Zoom with Q/E
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
            zoom += 0.02f;
            if (zoom > 3.5f) zoom = 3.5f;  // Max 350%
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
            zoom -= 0.02f;
            if (zoom < 0.3f) zoom = 0.3f;
        }
        
        // Clamp camera - bounds scale with zoom for proper panning when zoomed in
        // Use DYNAMIC map dimensions based on actual sector sizes
        float mapWidth = cityMap.getMapWidth();
        float mapHeight = cityMap.getMapHeight();
        
        // Allow generous panning across the entire map (full dimensions + buffer)
        float maxX = (mapWidth + 2000.0f) * zoom;
        float maxY = (mapHeight + 2000.0f) * zoom;
        
        if (camX > maxX) camX = maxX;
        if (camX < -maxX) camX = -maxX;
        if (camY > maxY) camY = maxY;
        if (camY < -maxY) camY = -maxY;
    }
    
    void handleClick(int screenX, int screenY) {
        if (screenX < UI_WIDTH) return;  // UI area
        
        // Convert screen to world coordinates
        sf::View mapView = getMapView();
        sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(screenX, screenY), mapView);
        
        // Check sector
        const char* sector = cityMap.getSectorAt(worldPos.x, worldPos.y);
        if (sector) {
            strcpy(currentSector, sector);
        } else {
            strcpy(currentSector, "None");
        }
        
        // ===== PATHFINDING MODE CLICK HANDLING =====
        if (pathfindingMode) {
            // Handle clicks based on network type
            int entityCount = 0;
            float clickRadius = 70.0f;
            
            if (pathfindingNetworkType == 0) {
                entityCount = stopCount;
                clickRadius = 70.0f;   // Bus stops: 150px sprite / 2 = 75px
            } else if (pathfindingNetworkType == 1) {
                entityCount = airportCount;
                clickRadius = 120.0f;  // Airports: 260px sprite / 2 = 130px
            } else if (pathfindingNetworkType == 2) {
                entityCount = railwayStationCount;
                clickRadius = 100.0f;  // Railways: 220px sprite / 2 = 110px
            }
            
            for (int i = 0; i < entityCount; i++) {
                float entityX, entityY;
                if (pathfindingNetworkType == 0) {
                    entityX = stops[i].x;
                    entityY = stops[i].y;
                } else if (pathfindingNetworkType == 1) {
                    entityX = airports[i].x;
                    entityY = airports[i].y;
                } else {
                    entityX = railwayStations[i].x;
                    entityY = railwayStations[i].y;
                }
                
                float dx = worldPos.x - entityX;
                float dy = worldPos.y - entityY;
                if (std::sqrt(dx*dx + dy*dy) < clickRadius) {
                    if (pathSourceStop < 0) {
                        // First click - select source
                        pathSourceStop = i;
                        pathDestStop = -1;
                        pathStopCount = 0;
                        pathFound = false;
                    } else if (pathDestStop < 0 && i != pathSourceStop) {
                        // Second click - select destination and calculate
                        pathDestStop = i;
                        calculatePath();
                    } else {
                        // Click again to start over
                        pathSourceStop = i;
                        pathDestStop = -1;
                        pathStopCount = 0;
                        pathFound = false;
                    }
                    return;
                }
            }
            return;  // Don't process other entities in pathfinding mode
        }
        
        // Check stops
        selectedStop = -1;
        for (int i = 0; i < stopCount; i++) {
            float dx = worldPos.x - stops[i].x;
            float dy = worldPos.y - stops[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 80.0f) {
                selectedStop = i;
                selectedHospital = -1;
                selectedSchool = -1;
                return;
            }
        }
        
        // Check hospitals
        selectedHospital = -1;
        for (int i = 0; i < hospitalCount; i++) {
            float dx = worldPos.x - hospitals[i].x;
            float dy = worldPos.y - hospitals[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 80.0f) {  // Tight click area
                selectedHospital = i;
                selectedStop = -1;
                selectedSchool = -1;
                return;
            }
        }
        
        // Check schools
        selectedSchool = -1;
        for (int i = 0; i < schoolCount; i++) {
            float dx = worldPos.x - schools[i].x;
            float dy = worldPos.y - schools[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 120.0f) {
                selectedSchool = i;
                selectedStop = -1;
                selectedHospital = -1;
                return;
            }
        }
        
        // Helper lambda to clear all selections
        auto clearSelections = [&]() {
            selectedStop = -1;
            selectedHospital = -1;
            selectedSchool = -1;
            selectedPharmacy = -1;
            selectedMall = -1;
            selectedPark = -1;
            selectedMosque = -1;
            selectedAirport = -1;
            selectedRailwayStation = -1;
            selectedBus = -1;
            selectedTrain = -1;
            selectedFlight = -1;
        };
        
        // Check pharmacies
        for (int i = 0; i < pharmacyCount; i++) {
            float dx = worldPos.x - pharmacies[i].x;
            float dy = worldPos.y - pharmacies[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 90.0f) {  // 180px sprite / 2
                clearSelections();
                selectedPharmacy = i;
                return;
            }
        }
        
        // Check malls
        for (int i = 0; i < mallCount; i++) {
            float dx = worldPos.x - malls[i].x;
            float dy = worldPos.y - malls[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 150.0f) {
                clearSelections();
                selectedMall = i;
                return;
            }
        }
        
        // Check parks
        for (int i = 0; i < parkCount; i++) {
            float dx = worldPos.x - parks[i].x;
            float dy = worldPos.y - parks[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 100.0f) {  // Tight click area
                clearSelections();
                selectedPark = i;
                return;
            }
        }
        
        // Check mosques
        for (int i = 0; i < mosqueCount; i++) {
            float dx = worldPos.x - mosques[i].x;
            float dy = worldPos.y - mosques[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 70.0f) {  // Tight click area
                clearSelections();
                selectedMosque = i;
                return;
            }
        }
        
        // Check airports (reduced radius to match visual size)
        for (int i = 0; i < airportCount; i++) {
            float dx = worldPos.x - airports[i].x;
            float dy = worldPos.y - airports[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 130.0f) {  // 260px sprite / 2
                clearSelections();
                selectedAirport = i;
                return;
            }
        }
        
        // Check railway stations (reduced radius to match visual size)
        for (int i = 0; i < railwayStationCount; i++) {
            float dx = worldPos.x - railwayStations[i].x;
            float dy = worldPos.y - railwayStations[i].y;
            if (std::sqrt(dx*dx + dy*dy) < 110.0f) {  // 220px sprite / 2
                clearSelections();
                selectedRailwayStation = i;
                return;
            }
        }
        
        // Check buses (smaller entities, check after stations)
        for (int i = 0; i < busCount; i++) {
            float dx = worldPos.x - buses[i].animX;
            float dy = worldPos.y - buses[i].animY;
            if (std::sqrt(dx*dx + dy*dy) < 100.0f) {
                clearSelections();
                selectedBus = i;
                return;
            }
        }
        
        // Check trains
        for (int i = 0; i < trainCount; i++) {
            float dx = worldPos.x - trains[i].animX;
            float dy = worldPos.y - trains[i].animY;
            if (std::sqrt(dx*dx + dy*dy) < 75.0f) {  // 150px sprite / 2
                clearSelections();
                selectedTrain = i;
                return;
            }
        }
        
        // Check flights
        for (int i = 0; i < flightCount; i++) {
            float dx = worldPos.x - flights[i].animX;
            float dy = worldPos.y - flights[i].animY;
            if (std::sqrt(dx*dx + dy*dy) < 65.0f) {  // 130px sprite / 2
                clearSelections();
                selectedFlight = i;
                return;
            }
        }
        
        // If nothing was clicked, clear all selections (deselect airports/railways)
        clearSelections();
    }
    
    sf::View getMapView() {
        sf::View view = window.getDefaultView();
        sf::Vector2f center(960.0f - camX, 540.0f - camY);
        view.setCenter(center);
        view.setSize(sf::Vector2f(1920.0f / zoom, 1080.0f / zoom));
        return view;
    }
    
    // Helper to batch sprites
    void appendSpriteToVertexArray(sf::VertexArray& va, float x, float y, float scale, const sf::Texture& tex, float opacity) {
        sf::Vector2u size = tex.getSize();
        float w = (float)size.x;
        float h = (float)size.y;
        
        // Calculate centered position with scale
        // Origin is center, so:
        float halfW = w * scale / 2.0f;
        float halfH = h * scale / 2.0f;
        
        float left = x - halfW;
        float right = x + halfW;
        float top = y - halfH;
        float bottom = y + halfH;
        
        sf::Color color(255, 255, 255, (unsigned char)opacity);
        
        // Add 6 vertices (2 triangles) for a quad
        // Triangle 1: Top-Left, Top-Right, Bottom-Left
        va.append(sf::Vertex(sf::Vector2f(left, top), color, sf::Vector2f(0.f, 0.f)));
        va.append(sf::Vertex(sf::Vector2f(right, top), color, sf::Vector2f(w, 0.f)));
        va.append(sf::Vertex(sf::Vector2f(left, bottom), color, sf::Vector2f(0.f, h)));
        
        // Triangle 2: Top-Right, Bottom-Right, Bottom-Left
        va.append(sf::Vertex(sf::Vector2f(right, top), color, sf::Vector2f(w, 0.f)));
        va.append(sf::Vertex(sf::Vector2f(right, bottom), color, sf::Vector2f(w, h)));
        va.append(sf::Vertex(sf::Vector2f(left, bottom), color, sf::Vector2f(0.f, h)));
    }

    void render() {
        window.clear(sf::Color(25, 30, 35));
        
        // Set map view for world rendering
        sf::View mapView = getMapView();
        window.setView(mapView);
        
        // Calculate view bounds once for culling (with buffer for large sprites)
        sf::Vector2f viewCenter = mapView.getCenter();
        sf::Vector2f viewSize = mapView.getSize();
        float viewLeft = viewCenter.x - viewSize.x / 2 - 300;
        float viewRight = viewCenter.x + viewSize.x / 2 + 300;
        float viewTop = viewCenter.y - viewSize.y / 2 - 300;
        float viewBottom = viewCenter.y + viewSize.y / 2 + 300;
        
        // Helper lambda for view culling check
        auto isInView = [&](float x, float y) {
            return x >= viewLeft && x <= viewRight && y >= viewTop && y <= viewBottom;
        };
        
        // 1. Background
        renderer.drawBackground(window, cityMap.getMapWidth(), cityMap.getMapHeight());
        
        // 2. Sector zones
        cityMap.drawZones(window);
        
        // 3. Connections
        // 3. Bus connections (yellow)
        if (showConnections) {
            for (int i = 0; i < connectionCount; i++) {
                int from = connections[i].fromIdx;
                int to = connections[i].toIdx;
                bool highlight = (from == selectedStop || to == selectedStop);
                renderer.drawConnection(window, stops[from].x, stops[from].y,
                                        stops[to].x, stops[to].y, highlight);
            }
        }
        
        // ===== PATHFINDING PATH RENDERING =====
        if (pathfindingMode && pathFound && pathStopCount > 1) {
            // Use bright green for ALL highlighted paths (buses, airports, railways)
            sf::Color pathColor = sf::Color(50, 255, 100, 220);
            
            // Draw the calculated path
            for (int i = 0; i < pathStopCount - 1; i++) {
                int fromIdx = -1, toIdx = -1;
                
                // Find entity indices based on network type
                if (pathfindingNetworkType == 0) {
                    for (int j = 0; j < stopCount; j++) {
                        if (strcmp(stops[j].id, pathStopIds[i]) == 0) fromIdx = j;
                        if (strcmp(stops[j].id, pathStopIds[i + 1]) == 0) toIdx = j;
                    }
                } else if (pathfindingNetworkType == 1) {
                    for (int j = 0; j < airportCount; j++) {
                        if (strcmp(airports[j].id, pathStopIds[i]) == 0) fromIdx = j;
                        if (strcmp(airports[j].id, pathStopIds[i + 1]) == 0) toIdx = j;
                    }
                } else {
                    for (int j = 0; j < railwayStationCount; j++) {
                        if (strcmp(railwayStations[j].id, pathStopIds[i]) == 0) fromIdx = j;
                        if (strcmp(railwayStations[j].id, pathStopIds[i + 1]) == 0) toIdx = j;
                    }
                }
                
                if (fromIdx >= 0 && toIdx >= 0) {
                    float x1, y1, x2, y2;
                    if (pathfindingNetworkType == 0) {
                        x1 = stops[fromIdx].x; y1 = stops[fromIdx].y;
                        x2 = stops[toIdx].x; y2 = stops[toIdx].y;
                    } else if (pathfindingNetworkType == 1) {
                        x1 = airports[fromIdx].x; y1 = airports[fromIdx].y;
                        x2 = airports[toIdx].x; y2 = airports[toIdx].y;
                    } else {
                        x1 = railwayStations[fromIdx].x; y1 = railwayStations[fromIdx].y;
                        x2 = railwayStations[toIdx].x; y2 = railwayStations[toIdx].y;
                    }
                    
                    float dx = x2 - x1, dy = y2 - y1;
                    float len = std::sqrt(dx*dx + dy*dy);
                    if (len > 1) {
                        float thickness = 10.0f;
                        float nx = -dy / len * thickness / 2;
                        float ny = dx / len * thickness / 2;
                        
                        sf::ConvexShape pathLine(4);
                        pathLine.setPoint(0, sf::Vector2f(x1 + nx, y1 + ny));
                        pathLine.setPoint(1, sf::Vector2f(x1 - nx, y1 - ny));
                        pathLine.setPoint(2, sf::Vector2f(x2 - nx, y2 - ny));
                        pathLine.setPoint(3, sf::Vector2f(x2 + nx, y2 + ny));
                        pathLine.setFillColor(pathColor);
                        window.draw(pathLine);
                    }
                }
            }
        }
        
        // Draw source/destination markers for pathfinding
        if (pathfindingMode) {
            float srcX = 0, srcY = 0, destX = 0, destY = 0;
            sf::Color srcOutline, destOutline;
            
            // Get positions and colors based on network type
            if (pathfindingNetworkType == 0) {
                srcOutline = sf::Color(0, 255, 0);
                destOutline = sf::Color(255, 0, 0);
                if (pathSourceStop >= 0) { srcX = stops[pathSourceStop].x; srcY = stops[pathSourceStop].y; }
                if (pathDestStop >= 0) { destX = stops[pathDestStop].x; destY = stops[pathDestStop].y; }
            } else if (pathfindingNetworkType == 1) {
                srcOutline = sf::Color(255, 200, 0);
                destOutline = sf::Color(255, 100, 0);
                if (pathSourceStop >= 0) { srcX = airports[pathSourceStop].x; srcY = airports[pathSourceStop].y; }
                if (pathDestStop >= 0) { destX = airports[pathDestStop].x; destY = airports[pathDestStop].y; }
            } else {
                srcOutline = sf::Color(200, 150, 255);
                destOutline = sf::Color(147, 51, 234);
                if (pathSourceStop >= 0) { srcX = railwayStations[pathSourceStop].x; srcY = railwayStations[pathSourceStop].y; }
                if (pathDestStop >= 0) { destX = railwayStations[pathDestStop].x; destY = railwayStations[pathDestStop].y; }
            }
            
            if (pathSourceStop >= 0) {
                sf::CircleShape sourceMarker(25.0f);
                sourceMarker.setOrigin(sf::Vector2f(25.0f, 25.0f));
                sourceMarker.setPosition(sf::Vector2f(srcX, srcY));
                sourceMarker.setFillColor(sf::Color(srcOutline.r, srcOutline.g, srcOutline.b, 100));
                sourceMarker.setOutlineColor(srcOutline);
                sourceMarker.setOutlineThickness(4.0f);
                window.draw(sourceMarker);
            }
            if (pathDestStop >= 0) {
                sf::CircleShape destMarker(25.0f);
                destMarker.setOrigin(sf::Vector2f(25.0f, 25.0f));
                destMarker.setPosition(sf::Vector2f(destX, destY));
                destMarker.setFillColor(sf::Color(destOutline.r, destOutline.g, destOutline.b, 100));
                destMarker.setOutlineColor(destOutline);
                destMarker.setOutlineThickness(4.0f);
                window.draw(destMarker);
            }
        }
        
        // Airport connections (orange) - highlight if airport is selected
        if (showAirportConnections) {
            for (int i = 0; i < airportConnectionCount; i++) {
                int from = airportConnections[i].fromIdx;
                int to = airportConnections[i].toIdx;
                bool highlight = (from == selectedAirport || to == selectedAirport);
                if (highlight) {
                    // Draw bright orange for highlighted connections
                    renderer.drawConnection(window, airports[from].x, airports[from].y,
                                          airports[to].x, airports[to].y, true);
                } else {
                    renderer.drawAirportConnection(window, airports[from].x, airports[from].y,
                                                   airports[to].x, airports[to].y);
                }
            }
        }
        
        // Railway connections (purple) - highlight if station is selected
        if (showRailwayConnections) {
            for (int i = 0; i < railwayConnectionCount; i++) {
                int from = railwayConnections[i].fromIdx;
                int to = railwayConnections[i].toIdx;
                bool highlight = (from == selectedRailwayStation || to == selectedRailwayStation);
                if (highlight) {
                    // Draw bright for highlighted connections
                    renderer.drawConnection(window, railwayStations[from].x, railwayStations[from].y,
                                          railwayStations[to].x, railwayStations[to].y, true);
                } else {
                    renderer.drawRailwayConnection(window, railwayStations[from].x, railwayStations[from].y,
                                                   railwayStations[to].x, railwayStations[to].y);
                }
            }
        }
        
        // 3.5 Decorations (Batched Optimization)
        if (showDecorations) {
            // Clear previous vertices
            for(int i=0; i<8; i++) vaDecorations[i].clear();
            
            // Batch visible decorations
            for (int i = 0; i < decorationCount; i++) {
                if (isInView(decorations[i].x, decorations[i].y)) {
                    int type = decorations[i].type;
                    if(type >= 0 && type < 8) {
                        const sf::Texture* tex = renderer.getDecorationTexture(type);
                        if(tex && tex->getSize().x > 0) {
                            float targetSize = 140.0f;
                            switch(type) {
                                case 0: targetSize = 140.0f; break; // House
                                case 1: targetSize = 180.0f; break; // Flats
                                case 2: targetSize = 200.0f; break; // Factory
                                case 3: targetSize = 120.0f; break; // Water Tower
                                case 4: targetSize = 160.0f; break; // Restaurant
                                case 5: targetSize = 180.0f; break; // Gas Station
                                case 6: targetSize = 180.0f; break; // Police Station
                                case 7: targetSize = 180.0f; break; // Fire Station
                                default: targetSize = 140.0f;
                            }
                            float scale = targetSize / (float)tex->getSize().x;
                            appendSpriteToVertexArray(vaDecorations[type], decorations[i].x, decorations[i].y, scale, *tex, 255.0f);
                        }
                    }
                }
            }
            
            // Draw batches (8 draw calls max instead of 200+)
            for(int i=0; i<8; i++) {
                if(vaDecorations[i].getVertexCount() > 0) {
                    sf::RenderStates states;
                    states.texture = renderer.getDecorationTexture(i);
                    window.draw(vaDecorations[i], states);
                }
            }
        }
        
        // 4. Hospitals
        if (showHospitals) {
            for (int i = 0; i < hospitalCount; i++) {
                if (isInView(hospitals[i].x, hospitals[i].y)) {
                    renderer.drawHospital(window, hospitals[i].x, hospitals[i].y);
                }
            }
        }
        
        // 5. Schools
        if (showSchools) {
            for (int i = 0; i < schoolCount; i++) {
                if (isInView(schools[i].x, schools[i].y)) {
                    renderer.drawSchool(window, schools[i].x, schools[i].y);
                }
            }
        }
        
        // 6. Pharmacies
        if (showPharmacies) {
            for (int i = 0; i < pharmacyCount; i++) {
                if (isInView(pharmacies[i].x, pharmacies[i].y)) {
                    renderer.drawPharmacy(window, pharmacies[i].x, pharmacies[i].y);
                }
            }
        }
        
        // 7. Malls
        if (showMalls) {
            for (int i = 0; i < mallCount; i++) {
                if (isInView(malls[i].x, malls[i].y)) {
                    renderer.drawMall(window, malls[i].x, malls[i].y);
                }
            }
        }
        
        // 8. Parks
        if (showParks) {
            for (int i = 0; i < parkCount; i++) {
                if (isInView(parks[i].x, parks[i].y)) {
                    renderer.drawPark(window, parks[i].x, parks[i].y);
                }
            }
        }
        
        // Mosques
        if (showMosques) {
            for (int i = 0; i < mosqueCount; i++) {
                if (isInView(mosques[i].x, mosques[i].y)) {
                    renderer.drawMosque(window, mosques[i].x, mosques[i].y);
                }
            }
        }
        
        // 9. Airports
        if (showAirports) {
            for (int i = 0; i < airportCount; i++) {
                if (isInView(airports[i].x, airports[i].y)) {
                    renderer.drawAirport(window, airports[i].x, airports[i].y);
                }
            }
        }
        
        // 10. Railway stations
        if (showRailways) {
            for (int i = 0; i < railwayStationCount; i++) {
                if (isInView(railwayStations[i].x, railwayStations[i].y)) {
                    renderer.drawTrainStation(window, railwayStations[i].x, railwayStations[i].y);
                }
            }
        }
        
        // 11. Buses
        if (showBuses) {
            for (int i = 0; i < busCount; i++) {
                if (isInView(buses[i].animX, buses[i].animY)) {
                    renderer.drawBus(window, buses[i].animX, buses[i].animY);
                }
            }
        }
        
        // 12. Trains
        if (showTrains) {
            for (int i = 0; i < trainCount; i++) {
                if (isInView(trains[i].animX, trains[i].animY)) {
                    renderer.drawTrain(window, trains[i].animX, trains[i].animY);
                }
            }
        }
        
        // 13. Airplanes
        if (showFlights) {
            for (int i = 0; i < flightCount; i++) {
                if (isInView(flights[i].animX, flights[i].animY)) {
                    renderer.drawAirplane(window, flights[i].animX, flights[i].animY);
                }
            }
        }
        
        // 13b. Animated People (Batched Optimization)
        if (showPeople) {
            // Clear buffers
            for(int i=0; i<6; i++) vaPeople[i].clear();
            
            // Batch visible people
            for (int i = 0; i < personCount; i++) {
                if (isInView(people[i].x, people[i].y)) {
                    sf::Texture* pTex = nullptr;
                    switch(people[i].type) {
                        case 0: pTex = &texPersonM; break;
                        case 1: pTex = &texPersonF; break;
                        case 2: pTex = &texPolice; break;
                        case 3: pTex = &texDoctor; break;
                        case 4: pTex = &texStudent; break;
                        case 5: pTex = &texEngineer; break;
                        default: pTex = &texPersonM; break;
                    }
                    
                    if(pTex && pTex->getSize().x > 0) {
                        appendSpriteToVertexArray(vaPeople[people[i].type], people[i].x, people[i].y, 0.08f, *pTex, people[i].opacity);
                    }
                }
            }
            
            // Draw batches (6 draw calls max instead of 500+)
            for(int i=0; i<6; i++) {
                if(vaPeople[i].getVertexCount() > 0) {
                    sf::Texture* pTex = nullptr;
                    switch(i) {
                        case 0: pTex = &texPersonM; break;
                        case 1: pTex = &texPersonF; break;
                        case 2: pTex = &texPolice; break;
                        case 3: pTex = &texDoctor; break;
                        case 4: pTex = &texStudent; break;
                        case 5: pTex = &texEngineer; break;
                    }
                    
                    if(pTex) {
                        sf::RenderStates states;
                        states.texture = pTex;
                        window.draw(vaPeople[i], states);
                    }
                }
            }
        }
        
        // 14. Bus stops (drawn last for visibility)
        for (int i = 0; i < stopCount; i++) {
            if (isInView(stops[i].x, stops[i].y)) {
                renderer.drawBusStop(window, stops[i].x, stops[i].y, i == selectedStop);
            }
        }
        
        // 15. Sector labels (LAST in world space - always on top)
        cityMap.drawLabels(window, font);
        
        // Reset to default view for UI
        window.setView(window.getDefaultView());
        
        // Draw UI panel
        drawUI();
        
        window.display();
    }
    
    void drawUI() {
        // Background panel
        sf::RectangleShape panel(sf::Vector2f(UI_WIDTH, 1080.0f));
        panel.setFillColor(sf::Color(30, 40, 50, 240));
        window.draw(panel);
        
        // Title - ISB in red, GLYPH in white (two separate text objects)
        sf::Text titleISB(font, "ISB", 28);
        titleISB.setPosition(sf::Vector2f(15, 12));
        titleISB.setFillColor(sf::Color(255, 80, 80));
        titleISB.setStyle(sf::Text::Bold);
        window.draw(titleISB);
        
        sf::Text titleGLYPH(font, "GLYPH", 28);
        titleGLYPH.setPosition(sf::Vector2f(65, 12));
        titleGLYPH.setFillColor(sf::Color::White);
        titleGLYPH.setStyle(sf::Text::Bold);
        window.draw(titleGLYPH);
        
        // ===== SEARCH BOX =====
        float searchY = 55;
        sf::RectangleShape searchBox(sf::Vector2f(UI_WIDTH - 30, 38));
        searchBox.setPosition(sf::Vector2f(15, searchY));
        searchBox.setFillColor(sf::Color(40, 50, 60));
        searchBox.setOutlineThickness(2);
        searchBox.setOutlineColor(searchBoxFocused ? sf::Color(100, 180, 255) : sf::Color(60, 70, 80));
        window.draw(searchBox);
        
        // Search text or placeholder
        if (searchQuery[0] != '\0') {
            sf::Text searchText(font, searchQuery, 16);
            searchText.setPosition(sf::Vector2f(20, searchY + 9));
            searchText.setFillColor(sf::Color::White);
            window.draw(searchText);
        } else if (!searchBoxFocused) {
            sf::Text placeholder(font, "Press / to search...", 16);
            placeholder.setPosition(sf::Vector2f(20, searchY + 9));
            placeholder.setFillColor(sf::Color(120, 120, 120));
            window.draw(placeholder);
        }
        
        // Search results dropdown
        if (searchBoxFocused && searchResultCount > 0) {
            float dropdownY = searchY + 40;
            int maxResults = std::min(searchResultCount, 3);  // Show max 3 for cleaner UI
            float dropdownHeight = maxResults * 30.0f;
            
            sf::RectangleShape dropdown(sf::Vector2f(UI_WIDTH - 30, dropdownHeight));
            dropdown.setPosition(sf::Vector2f(15, dropdownY));
            dropdown.setFillColor(sf::Color(50, 60, 70, 250));
            dropdown.setOutlineThickness(1);
            dropdown.setOutlineColor(sf::Color(80, 90, 100));
            window.draw(dropdown);
            
            // Draw each result
            for (int i = 0; i < maxResults; i++) {
                float itemY = dropdownY + i * 30.0f;
                
                // Highlight hovered or selected
                if (i == hoveredSearchResult || i == selectedSearchResult) {
                    sf::RectangleShape highlight(sf::Vector2f(UI_WIDTH - 30, 28));
                    highlight.setPosition(sf::Vector2f(15, itemY + 1));
                    highlight.setFillColor(sf::Color(70, 90, 120, 200));
                    window.draw(highlight);
                }
                
                // Entity type indicator (simple text - emojis don't render in most fonts)
                const char* typeNames[] = {"[S]", "[H]", "[E]", "[P]", "[M]", "[Pk]", "[Mq]", "[A]", "[R]", "[B]", "[T]", "[F]"};
                sf::Text typeIcon(font, typeNames[searchResults[i].entityType], 10);
                typeIcon.setPosition(sf::Vector2f(20, itemY + 8));
                window.draw(typeIcon);
                
                // Result name (truncate if too long)
                char displayName[40];
                if (strlen(searchResults[i].name) > 25) {
                    strncpy(displayName, searchResults[i].name, 22);
                    displayName[22] = '.';
                    displayName[23] = '.';
                    displayName[24] = '\0';
                } else {
                    strcpy(displayName, searchResults[i].name);
                }
                
                sf::Text resultText(font, displayName, 12);
                resultText.setPosition(sf::Vector2f(50, itemY + 6));
                resultText.setFillColor(sf::Color(220, 220, 220));
                window.draw(resultText);
                
                // Match score indicator
                sf::Color scoreColor = searchResults[i].matchScore >= 75 ? sf::Color(100, 255, 100) : 
                                       searchResults[i].matchScore >= 50 ? sf::Color(255, 200, 100) : 
                                       sf::Color(180, 180, 180);
                sf::Text scoreText(font, std::to_string(searchResults[i].matchScore).c_str(), 10);
                scoreText.setPosition(sf::Vector2f(UI_WIDTH - 40, itemY + 8));
                scoreText.setFillColor(scoreColor);
                window.draw(scoreText);
            }
        }
        
        // Current sector - position below search area with proper spacing
        float contentStartY = searchBoxFocused && searchResultCount > 0 ? 
                              100 + std::min(searchResultCount, 3) * 30.0f : 110;
        
        sf::Text sectorText(font, currentSector, 18);
        sectorText.setPosition(sf::Vector2f(15, contentStartY));
        sectorText.setFillColor(sf::Color(255, 200, 100));
        sectorText.setStyle(sf::Text::Bold);
        window.draw(sectorText);
        
        // Stats - positioned below sector with spacing
        char statsStr[200];
        snprintf(statsStr, 200, "S:%d | H:%d | E:%d | Zoom:%.0f%%", 
                 stopCount, hospitalCount, schoolCount, zoom * 100);
        sf::Text statsText(font, statsStr, 14);
        statsText.setPosition(sf::Vector2f(15, contentStartY + 28));
        statsText.setFillColor(sf::Color(180, 180, 180));
        window.draw(statsText);
        
        // ===== INSTRUCTIONS PANEL =====
        float panelY = 200.0f;
        
        if (showInstructions) {
            sf::RectangleShape instrPanel(sf::Vector2f(230, 520));
            instrPanel.setPosition(sf::Vector2f(10, panelY));
            instrPanel.setFillColor(sf::Color(35, 45, 55, 250));
            instrPanel.setOutlineColor(sf::Color(100, 150, 200));
            instrPanel.setOutlineThickness(2.0f);
            window.draw(instrPanel);
            
            sf::Text instrTitle(font, "KEYBOARD CONTROLS", 16);
            instrTitle.setPosition(sf::Vector2f(20, panelY + 10));
            instrTitle.setFillColor(sf::Color(255, 200, 100));
            instrTitle.setStyle(sf::Text::Bold);
            window.draw(instrTitle);
            
            // List of controls
            const char* controls[] = {
                "WASD / Arrows - Pan camera",
                "Mouse Scroll - Zoom in/out",
                "Q/E - Zoom out/in",
                "/ - Search entities",
                "Space - Pause/Resume animation",
                "R - Reset vehicles",
                "",
                "TOGGLE LAYERS (show/hide):",
                "2 - Hospitals",
                "3 - Schools",
                "4 - Pharmacies",
                "5 - Malls",
                "6 - Parks",
                "7 - Airports",
                "8 - Railways",
                "9 - Buses",
                "0 - Trains",
                "- - Flights",
                "= - Mosques",
                "; - Decorations",
                "",
                "1 - Cycle connection lines",
                "P - Pathfinding mode",
                "I - Toggle this panel"
            };
            
            float lineY = panelY + 35;
            for (int i = 0; i < 24; i++) {
                sf::Color textColor = (i == 0 || i == 7 || i == 21) ? sf::Color(150, 200, 255) : sf::Color(200, 200, 200);
                int fontSize = (i == 7) ? 13 : 12;
                sf::Text ctrlText(font, controls[i], fontSize);
                ctrlText.setPosition(sf::Vector2f(20, lineY));
                ctrlText.setFillColor(textColor);
                if (i == 7) ctrlText.setStyle(sf::Text::Bold);
                window.draw(ctrlText);
                lineY += (i == 6 || i == 20) ? 8 : 18;
            }
            
            panelY += 530;
        }
        
        // ===== PATHFINDING MODE UI =====
        if (!showInstructions) panelY = 200.0f;
        
        if (pathfindingMode) {
            // Network-specific colors and labels
            const char* networkName = pathfindingNetworkType == 0 ? "BUSES" :
                                      pathfindingNetworkType == 1 ? "AIRPORTS" : "RAILWAYS";
            sf::Color networkColor = pathfindingNetworkType == 0 ? sf::Color(50, 255, 100) :
                                     pathfindingNetworkType == 1 ? sf::Color(255, 180, 50) : sf::Color(180, 100, 255);
            sf::Color panelBg = pathfindingNetworkType == 0 ? sf::Color(40, 60, 50) :
                                pathfindingNetworkType == 1 ? sf::Color(60, 50, 40) : sf::Color(50, 40, 60);
            
            sf::RectangleShape pathPanel(sf::Vector2f(230, 130));
            pathPanel.setPosition(sf::Vector2f(10, panelY));
            pathPanel.setFillColor(panelBg);
            pathPanel.setOutlineColor(networkColor);
            pathPanel.setOutlineThickness(2.0f);
            window.draw(pathPanel);
            
            char modeStr[50];
            snprintf(modeStr, 50, "PATHFINDING: %s", networkName);
            sf::Text modeLabel(font, modeStr, 11);
            modeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            modeLabel.setFillColor(networkColor);
            modeLabel.setStyle(sf::Text::Bold);
            window.draw(modeLabel);
            
            // Instructions / Status
            const char* entityType = pathfindingNetworkType == 0 ? "stop" :
                                     pathfindingNetworkType == 1 ? "airport" : "station";
            char instructionStr[60];
            if (pathSourceStop < 0) {
                snprintf(instructionStr, 60, "Click source %s", entityType);
            } else if (pathDestStop < 0) {
                snprintf(instructionStr, 60, "Click destination %s", entityType);
            } else if (pathFound) {
                snprintf(instructionStr, 60, "Path found!");
            } else {
                snprintf(instructionStr, 60, "No path found");
            }
            sf::Text instructText(font, instructionStr, 12);
            instructText.setPosition(sf::Vector2f(15, panelY + 25));
            instructText.setFillColor(sf::Color(200, 200, 200));
            window.draw(instructText);
            
            // Source/Dest names based on network type
            if (pathSourceStop >= 0) {
                char srcStr[80];
                if (pathfindingNetworkType == 0) {
                    snprintf(srcStr, 80, "From: %s", stops[pathSourceStop].name);
                } else if (pathfindingNetworkType == 1) {
                    snprintf(srcStr, 80, "From: %s", airports[pathSourceStop].name);
                } else {
                    snprintf(srcStr, 80, "From: %s", railwayStations[pathSourceStop].name);
                }
                sf::Text srcText(font, srcStr, 11);
                srcText.setPosition(sf::Vector2f(15, panelY + 45));
                srcText.setFillColor(networkColor);
                window.draw(srcText);
            }
            if (pathDestStop >= 0) {
                char destStr[80];
                if (pathfindingNetworkType == 0) {
                    snprintf(destStr, 80, "To: %s", stops[pathDestStop].name);
                } else if (pathfindingNetworkType == 1) {
                    snprintf(destStr, 80, "To: %s", airports[pathDestStop].name);
                } else {
                    snprintf(destStr, 80, "To: %s", railwayStations[pathDestStop].name);
                }
                sf::Text destText(font, destStr, 11);
                destText.setPosition(sf::Vector2f(15, panelY + 65));
                destText.setFillColor(sf::Color(255, 200, 200));
                window.draw(destText);
            }
            
            // Distance
            if (pathFound) {
                char distStr[50];
                snprintf(distStr, 50, "Distance: %.1f km", pathDistance);
                sf::Text distText(font, distStr, 13);
                distText.setPosition(sf::Vector2f(15, panelY + 90));
                distText.setFillColor(sf::Color(255, 255, 100));
                distText.setStyle(sf::Text::Bold);
                window.draw(distText);
            }
            
            sf::Text escHint(font, "[P] change network/exit", 10);
            escHint.setPosition(sf::Vector2f(15, panelY + 112));
            escHint.setFillColor(sf::Color(120, 120, 120));
            window.draw(escHint);
            
            panelY += 140;  // Move entity panel below pathfinding panel
        }
        
        // Selected entity info - base Y position for all panels
        
        if (selectedStop >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(100, 180, 255));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "BUS STOP", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(100, 200, 255));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text stopName(font, stops[selectedStop].name, 13);
            stopName.setPosition(sf::Vector2f(15, panelY + 25));
            stopName.setFillColor(sf::Color::White);
            window.draw(stopName);
            
            sf::Text stopId(font, stops[selectedStop].id, 11);
            stopId.setPosition(sf::Vector2f(15, panelY + 50));
            stopId.setFillColor(sf::Color(180, 180, 180));
            window.draw(stopId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", stops[selectedStop].sector);
            sf::Text stopSector(font, sectorStr, 11);
            stopSector.setPosition(sf::Vector2f(15, panelY + 70));
            stopSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(stopSector);
        } else if (selectedHospital >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(255, 100, 100));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "HOSPITAL", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(255, 100, 100));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text hospName(font, hospitals[selectedHospital].name, 13);
            hospName.setPosition(sf::Vector2f(15, panelY + 25));
            hospName.setFillColor(sf::Color::White);
            window.draw(hospName);
            
            sf::Text hospId(font, hospitals[selectedHospital].id, 11);
            hospId.setPosition(sf::Vector2f(15, panelY + 50));
            hospId.setFillColor(sf::Color(180, 180, 180));
            window.draw(hospId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", hospitals[selectedHospital].sector);
            sf::Text hospSector(font, sectorStr, 11);
            hospSector.setPosition(sf::Vector2f(15, panelY + 70));
            hospSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(hospSector);
        } else if (selectedSchool >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(100, 255, 100));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "SCHOOL", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(100, 255, 100));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text schoolName(font, schools[selectedSchool].name, 13);
            schoolName.setPosition(sf::Vector2f(15, panelY + 25));
            schoolName.setFillColor(sf::Color::White);
            window.draw(schoolName);
            
            sf::Text schoolId(font, schools[selectedSchool].id, 11);
            schoolId.setPosition(sf::Vector2f(15, panelY + 50));
            schoolId.setFillColor(sf::Color(180, 180, 180));
            window.draw(schoolId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", schools[selectedSchool].sector);
            sf::Text schoolSector(font, sectorStr, 11);
            schoolSector.setPosition(sf::Vector2f(15, panelY + 70));
            schoolSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(schoolSector);
        } else if (selectedPharmacy >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(180, 100, 255));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "PHARMACY", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(180, 100, 255));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text pharmName(font, pharmacies[selectedPharmacy].name, 13);
            pharmName.setPosition(sf::Vector2f(15, panelY + 25));
            pharmName.setFillColor(sf::Color::White);
            window.draw(pharmName);
            
            sf::Text pharmId(font, pharmacies[selectedPharmacy].id, 11);
            pharmId.setPosition(sf::Vector2f(15, panelY + 50));
            pharmId.setFillColor(sf::Color(180, 180, 180));
            window.draw(pharmId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", pharmacies[selectedPharmacy].sector);
            sf::Text pharmSector(font, sectorStr, 11);
            pharmSector.setPosition(sf::Vector2f(15, panelY + 70));
            pharmSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(pharmSector);
        } else if (selectedMall >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(255, 200, 100));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "MALL", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(255, 200, 100));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text mallName(font, malls[selectedMall].name, 13);
            mallName.setPosition(sf::Vector2f(15, panelY + 25));
            mallName.setFillColor(sf::Color::White);
            window.draw(mallName);
            
            sf::Text mallId(font, malls[selectedMall].id, 11);
            mallId.setPosition(sf::Vector2f(15, panelY + 50));
            mallId.setFillColor(sf::Color(180, 180, 180));
            window.draw(mallId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", malls[selectedMall].sector);
            sf::Text mallSector(font, sectorStr, 11);
            mallSector.setPosition(sf::Vector2f(15, panelY + 70));
            mallSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(mallSector);
        } else if (selectedPark >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(100, 255, 150));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "PARK", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(100, 255, 150));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text parkName(font, parks[selectedPark].name, 13);
            parkName.setPosition(sf::Vector2f(15, panelY + 25));
            parkName.setFillColor(sf::Color::White);
            window.draw(parkName);
            
            sf::Text parkId(font, parks[selectedPark].id, 11);
            parkId.setPosition(sf::Vector2f(15, panelY + 50));
            parkId.setFillColor(sf::Color(180, 180, 180));
            window.draw(parkId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", parks[selectedPark].sector);
            sf::Text parkSector(font, sectorStr, 11);
            parkSector.setPosition(sf::Vector2f(15, panelY + 70));
            parkSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(parkSector);
        } else if (selectedMosque >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(100, 220, 255));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "MOSQUE", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(100, 220, 255));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text mosqueName(font, mosques[selectedMosque].name, 13);
            mosqueName.setPosition(sf::Vector2f(15, panelY + 25));
            mosqueName.setFillColor(sf::Color::White);
            window.draw(mosqueName);
            
            sf::Text mosqueId(font, mosques[selectedMosque].id, 11);
            mosqueId.setPosition(sf::Vector2f(15, panelY + 50));
            mosqueId.setFillColor(sf::Color(180, 180, 180));
            window.draw(mosqueId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", mosques[selectedMosque].sector);
            sf::Text mosqueSector(font, sectorStr, 11);
            mosqueSector.setPosition(sf::Vector2f(15, panelY + 70));
            mosqueSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(mosqueSector);
        } else if (selectedAirport >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 120));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(255, 140, 0));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "AIRPORT", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(255, 140, 0));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text airportName(font, airports[selectedAirport].name, 13);
            airportName.setPosition(sf::Vector2f(15, panelY + 25));
            airportName.setFillColor(sf::Color::White);
            window.draw(airportName);
            
            sf::Text airportId(font, airports[selectedAirport].id, 11);
            airportId.setPosition(sf::Vector2f(15, panelY + 50));
            airportId.setFillColor(sf::Color(180, 180, 180));
            window.draw(airportId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", airports[selectedAirport].sector);
            sf::Text airportSector(font, sectorStr, 11);
            airportSector.setPosition(sf::Vector2f(15, panelY + 70));
            airportSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(airportSector);
            
            sf::Text connHint(font, "Highlights routes", 10);
            connHint.setPosition(sf::Vector2f(15, panelY + 90));
            connHint.setFillColor(sf::Color(150, 150, 150));
            window.draw(connHint);
        } else if (selectedRailwayStation >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 120));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(147, 51, 234));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "RAILWAY STATION", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(147, 51, 234));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text stationName(font, railwayStations[selectedRailwayStation].name, 13);
            stationName.setPosition(sf::Vector2f(15, panelY + 25));
            stationName.setFillColor(sf::Color::White);
            window.draw(stationName);
            
            sf::Text stationId(font, railwayStations[selectedRailwayStation].id, 11);
            stationId.setPosition(sf::Vector2f(15, panelY + 50));
            stationId.setFillColor(sf::Color(180, 180, 180));
            window.draw(stationId);
            
            char sectorStr[60];
            snprintf(sectorStr, 60, "Sector: %s", railwayStations[selectedRailwayStation].sector);
            sf::Text stationSector(font, sectorStr, 11);
            stationSector.setPosition(sf::Vector2f(15, panelY + 70));
            stationSector.setFillColor(sf::Color(180, 180, 180));
            window.draw(stationSector);
            
            sf::Text connHint(font, "Highlights routes", 10);
            connHint.setPosition(sf::Vector2f(15, panelY + 90));
            connHint.setFillColor(sf::Color(150, 150, 150));
            window.draw(connHint);
        } else if (selectedBus >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(150, 200, 255));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "BUS", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(150, 200, 255));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text busId(font, buses[selectedBus].id, 13);
            busId.setPosition(sf::Vector2f(15, panelY + 25));
            busId.setFillColor(sf::Color::White);
            window.draw(busId);
            
            sf::Text busCompany(font, buses[selectedBus].name, 11);
            busCompany.setPosition(sf::Vector2f(15, panelY + 50));
            busCompany.setFillColor(sf::Color(180, 180, 180));
            window.draw(busCompany);
            
            char stopStr[60];
            snprintf(stopStr, 60, "At: %s", buses[selectedBus].currentStop);
            sf::Text busLocation(font, stopStr, 11);
            busLocation.setPosition(sf::Vector2f(15, panelY + 70));
            busLocation.setFillColor(sf::Color(180, 180, 180));
            window.draw(busLocation);
        } else if (selectedTrain >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(255, 120, 80));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "TRAIN", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(255, 120, 80));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text trainId(font, trains[selectedTrain].id, 13);
            trainId.setPosition(sf::Vector2f(15, panelY + 25));
            trainId.setFillColor(sf::Color::White);
            window.draw(trainId);
            
            sf::Text trainType(font, trains[selectedTrain].name, 11);
            trainType.setPosition(sf::Vector2f(15, panelY + 50));
            trainType.setFillColor(sf::Color(180, 180, 180));
            window.draw(trainType);
            
            char stationStr[60];
            snprintf(stationStr, 60, "At: %s", trains[selectedTrain].currentStation);
            sf::Text trainLocation(font, stationStr, 11);
            trainLocation.setPosition(sf::Vector2f(15, panelY + 70));
            trainLocation.setFillColor(sf::Color(180, 180, 180));
            window.draw(trainLocation);
        } else if (selectedFlight >= 0) {
            sf::RectangleShape infoBg(sf::Vector2f(230, 100));
            infoBg.setPosition(sf::Vector2f(10, panelY));
            infoBg.setFillColor(sf::Color(40, 50, 60));
            infoBg.setOutlineColor(sf::Color(120, 180, 255));
            infoBg.setOutlineThickness(2.0f);
            window.draw(infoBg);
            
            sf::Text typeLabel(font, "FLIGHT", 11);
            typeLabel.setPosition(sf::Vector2f(15, panelY + 5));
            typeLabel.setFillColor(sf::Color(120, 180, 255));
            typeLabel.setStyle(sf::Text::Bold);
            window.draw(typeLabel);
            
            sf::Text flightId(font, flights[selectedFlight].id, 13);
            flightId.setPosition(sf::Vector2f(15, panelY + 25));
            flightId.setFillColor(sf::Color::White);
            window.draw(flightId);
            
            sf::Text flightType(font, flights[selectedFlight].airline, 11);
            flightType.setPosition(sf::Vector2f(15, panelY + 50));
            flightType.setFillColor(sf::Color(180, 180, 180));
            window.draw(flightType);
            
            char airportStr[60];
            snprintf(airportStr, 60, "At: %s", flights[selectedFlight].currentAirport);
            sf::Text flightLocation(font, airportStr, 11);
            flightLocation.setPosition(sf::Vector2f(15, panelY + 70));
            flightLocation.setFillColor(sf::Color(180, 180, 180));
            window.draw(flightLocation);
        }
        
        // Help button at the bottom of the panel
        sf::RectangleShape helpBtn(sf::Vector2f(UI_WIDTH - 20, 35));
        helpBtn.setPosition(sf::Vector2f(10, 1030));
        helpBtn.setFillColor(showInstructions ? sf::Color(70, 100, 140) : sf::Color(45, 55, 65));
        helpBtn.setOutlineColor(sf::Color(80, 120, 180));
        helpBtn.setOutlineThickness(2.0f);
        window.draw(helpBtn);
        
        sf::Text helpBtnText(font, "[I] Help / Controls", 13);
        helpBtnText.setPosition(sf::Vector2f(65, 1039));
        helpBtnText.setFillColor(showInstructions ? sf::Color(255, 255, 255) : sf::Color(150, 180, 220));
        helpBtnText.setStyle(sf::Text::Bold);
        window.draw(helpBtnText);
    }
    
    void run() {
        while (running && window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }
    
    bool isRunning() { return running; }
};

#endif
