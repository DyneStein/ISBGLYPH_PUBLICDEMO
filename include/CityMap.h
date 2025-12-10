#ifndef CITYMAP_H
#define CITYMAP_H

#include <SFML/Graphics.hpp>
#include <cstring>
#include <cmath>

// Simple grid-based city layout - DYNAMIC sector loading
// Sectors auto-arranged in grid based on PopulationSystem data
// Now supports DYNAMIC SIZING based on entity count!

const float DEFAULT_CELL_WIDTH = 600.0f;
const float DEFAULT_CELL_HEIGHT = 500.0f;
const float GRID_START_X = 400.0f;
const float GRID_START_Y = 300.0f;
const float CELL_GAP_X = 50.0f;
const float CELL_GAP_Y = 50.0f;

// Sizing parameters for dynamic sectors
const int ENTITY_THRESHOLD = 2;       // Start expanding after this many entities
const float SIZE_SCALE_FACTOR = 60.0f; // Pixels added per extra entity

struct Sector {
    char name[50];
    sf::Color color;
    int row, col;
    float centerX, centerY;
    float width, height;      // Dynamic dimensions (can vary per sector)
    int entityCount;          // Number of entities in this sector
};

class CityMap {
private:
    Sector sectors[20];  // Support up to 20 sectors
    int sectorCount;
    int gridCols;  // Auto-calculated columns for nice layout
    
    // Generate color for sector based on name/index
    sf::Color generateColor(int index) {
        // Use different color schemes for F, G, H, I sectors
        int hue = (index * 40) % 360;
        int r = 100 + (index * 30) % 100;
        int g = 120 + (index * 50) % 100;
        int b = 100 + (index * 70) % 100;
        return sf::Color(r, g, b);
    }
    
public:
    CityMap() : sectorCount(0), gridCols(3) {}
    
    // Dynamically load sectors from PopulationSystem
    void loadSectorsFromSystem(void* popSystemPtr) {
        // Cast the population system pointer
        // (This will be called from Visualization with actual PopulationSystem*)
        
        // Forward declaration - implementation will be in Visualization where PopulationSystem is defined
        // For now, we'll just provide the interface
    }
    
    // Add sector to grid with default size
    void addSector(const char* name, sf::Color color, int row, int col) {
        if (sectorCount < 20) {
            strcpy(sectors[sectorCount].name, name);
            sectors[sectorCount].color = color;
            sectors[sectorCount].row = row;
            sectors[sectorCount].col = col;
            sectors[sectorCount].width = DEFAULT_CELL_WIDTH;
            sectors[sectorCount].height = DEFAULT_CELL_HEIGHT;
            sectors[sectorCount].entityCount = 0;
            // Calculate center from grid position (will be recalculated later)
            sectors[sectorCount].centerX = GRID_START_X + col * (DEFAULT_CELL_WIDTH + CELL_GAP_X) + DEFAULT_CELL_WIDTH / 2;
            sectors[sectorCount].centerY = GRID_START_Y + row * (DEFAULT_CELL_HEIGHT + CELL_GAP_Y) + DEFAULT_CELL_HEIGHT / 2;
            sectorCount++;
        }
    }
    
    // Set entity count for a sector and recalculate its size
    void setSectorEntityCount(const char* sectorName, int count) {
        char normalizedInput[100];
        normalizeSectorName(sectorName, normalizedInput);
        
        for (int i = 0; i < sectorCount; i++) {
            char normalizedSector[100];
            normalizeSectorName(sectors[i].name, normalizedSector);
            
            if (strcmp(normalizedSector, normalizedInput) == 0) {
                sectors[i].entityCount = count;
                
                // Calculate new size based on entity count
                if (count > ENTITY_THRESHOLD) {
                    int extra = count - ENTITY_THRESHOLD;
                    sectors[i].width = DEFAULT_CELL_WIDTH + extra * SIZE_SCALE_FACTOR;
                    sectors[i].height = DEFAULT_CELL_HEIGHT + extra * SIZE_SCALE_FACTOR * 0.8f;
                } else {
                    sectors[i].width = DEFAULT_CELL_WIDTH;
                    sectors[i].height = DEFAULT_CELL_HEIGHT;
                }
                return;
            }
        }
    }
    
    // Proactively expand a sector if an item failed to fit
    void expandSectorBounds(const char* sectorName, float extraSize) {
        char normalizedInput[100];
        normalizeSectorName(sectorName, normalizedInput);
        
        for (int i = 0; i < sectorCount; i++) {
            char normalizedSector[100];
            normalizeSectorName(sectors[i].name, normalizedSector);
            
            if (strcmp(normalizedSector, normalizedInput) == 0) {
                // Add extra buffer to accommodate the large item
                sectors[i].width += extraSize;
                sectors[i].height += extraSize * 0.8f;
                return;
            }
        }
    }
    
    // Recalculate all sector positions based on their individual sizes
    void recalculateGridPositions() {
        // Track row heights (max height in each row)
        float rowHeights[10] = {0};
        float colWidths[10] = {0};
        
        // First pass: find max height per row and max width per column
        for (int i = 0; i < sectorCount; i++) {
            int row = sectors[i].row;
            int col = sectors[i].col;
            if (sectors[i].height > rowHeights[row]) {
                rowHeights[row] = sectors[i].height;
            }
            if (sectors[i].width > colWidths[col]) {
                colWidths[col] = sectors[i].width;
            }
        }
        
        // Second pass: position sectors based on accumulated widths/heights
        for (int i = 0; i < sectorCount; i++) {
            int row = sectors[i].row;
            int col = sectors[i].col;
            
            // Calculate X position: sum of all column widths before this one
            float xPos = GRID_START_X;
            for (int c = 0; c < col; c++) {
                xPos += (colWidths[c] > 0 ? colWidths[c] : DEFAULT_CELL_WIDTH) + CELL_GAP_X;
            }
            
            // Calculate Y position: sum of all row heights before this one
            float yPos = GRID_START_Y;
            for (int r = 0; r < row; r++) {
                yPos += (rowHeights[r] > 0 ? rowHeights[r] : DEFAULT_CELL_HEIGHT) + CELL_GAP_Y;
            }
            
            // Center position is top-left + half the sector's own dimensions
            sectors[i].centerX = xPos + sectors[i].width / 2;
            sectors[i].centerY = yPos + sectors[i].height / 2;
        }
        
        printf("[OK] Recalculated grid positions for %d sectors\n", sectorCount);
    }
    
    // Auto-arrange sectors in grid based on sector list
    void buildGridFromSectors(char** sectorNames, int count) {
        sectorCount = 0;
        
        // Calculate grid dimensions (prefer 3-4 columns for nice layout)
        gridCols = 3;
        if (count > 12) gridCols = 4;
        
        // Add each sector to grid
        for (int i = 0; i < count && i < 20; i++) {
            int row = i / gridCols;
            int col = i % gridCols;
            sf::Color color = generateColor(i);
            addSector(sectorNames[i], color, row, col);
        }
        
        printf("[OK] CityMap built with %d sectors in %d columns\n", sectorCount, gridCols);
    }
    
    // Helper to normalize sector name (trim + uppercase)
    void normalizeSectorName(const char* input, char* output) {
        // Trim leading/trailing spaces and convert to uppercase
        int start = 0;
        while (input[start] == ' ') start++;
        
        int len = strlen(input + start);
        while (len > 0 && input[start + len - 1] == ' ') len--;
        
        // Copy trimmed string
        for (int i = 0; i < len; i++) {
            output[i] = input[start + i];
        }
        output[len] = '\0';
        
        // Convert to uppercase
        for (int i = 0; output[i]; i++) {
            if (output[i] >= 'a' && output[i] <= 'z') {
                output[i] = output[i] - 32;
            }
        }
    }
    
    // Get sector center position by name (case-insensitive, whitespace-trimmed)
    bool getSectorPosition(const char* name, float& x, float& y) {
        char normalizedInput[100];
        normalizeSectorName(name, normalizedInput);
        
        for (int i = 0; i < sectorCount; i++) {
            char normalizedSector[100];
            normalizeSectorName(sectors[i].name, normalizedSector);
            
            if (strcmp(normalizedSector, normalizedInput) == 0) {
                x = sectors[i].centerX;
                y = sectors[i].centerY;
                return true;
            }
        }
        return false;
    }
    
    // Get offset position within a sector - scales offsets based on sector size
    void getPositionInSector(const char* sectorName, float offsetX, float offsetY, float& outX, float& outY) {
        float cx, cy;
        if (getSectorPosition(sectorName, cx, cy)) {
            // Get sector dimensions to scale offsets
            float w, h;
            getSectorDimensions(sectorName, w, h);
            
            // Scale offsets proportionally to sector size
            float scaleX = w / DEFAULT_CELL_WIDTH;
            float scaleY = h / DEFAULT_CELL_HEIGHT;
            
            outX = cx + offsetX * scaleX;
            outY = cy + offsetY * scaleY;
        } else {
            // Default to first sector if not found (fallback)
            if (sectorCount > 0) {
                outX = sectors[0].centerX + offsetX;
                outY = sectors[0].centerY + offsetY;
            } else {
                outX = GRID_START_X + offsetX;
                outY = GRID_START_Y + offsetY;
            }
        }
    }
    
    // Draw sector zones (background layer) - uses per-sector dimensions
    void drawZones(sf::RenderWindow& window) {
        for (int i = 0; i < sectorCount; i++) {
            float x = sectors[i].centerX;
            float y = sectors[i].centerY;
            float w = sectors[i].width;
            float h = sectors[i].height;
            
            sf::RectangleShape zone(sf::Vector2f(w, h));
            zone.setPosition(sf::Vector2f(x - w/2, y - h/2));
            
            sf::Color fillColor = sectors[i].color;
            fillColor.a = 60;
            zone.setFillColor(fillColor);
            zone.setOutlineColor(sectors[i].color);
            zone.setOutlineThickness(3.0f);
            window.draw(zone);
        }
    }
    
    // Draw sector labels (TOP layer - call LAST) - uses per-sector dimensions
    void drawLabels(sf::RenderWindow& window, sf::Font& font) {
        for (int i = 0; i < sectorCount; i++) {
            float x = sectors[i].centerX;
            float y = sectors[i].centerY;
            float w = sectors[i].width;
            float h = sectors[i].height;
            
            // Position at top-left of zone
            float labelX = x - w/2 + 10;
            float labelY = y - h/2 + 5;
            
            // Background box
            sf::RectangleShape bg(sf::Vector2f(120.0f, 30.0f));
            bg.setPosition(sf::Vector2f(labelX - 5, labelY - 2));
            bg.setFillColor(sf::Color(20, 20, 20, 200));
            bg.setOutlineColor(sectors[i].color);
            bg.setOutlineThickness(2.0f);
            window.draw(bg);
            
            // Label text
            sf::Text label(font, sectors[i].name, 20);
            label.setPosition(sf::Vector2f(labelX, labelY));
            label.setFillColor(sf::Color::White);
            label.setStyle(sf::Text::Bold);
            window.draw(label);
        }
    }
    
    // Check which sector contains a point - uses per-sector dimensions
    const char* getSectorAt(float x, float y) {
        for (int i = 0; i < sectorCount; i++) {
            float cx = sectors[i].centerX;
            float cy = sectors[i].centerY;
            float w = sectors[i].width;
            float h = sectors[i].height;
            
            if (x >= cx - w/2 && x <= cx + w/2 &&
                y >= cy - h/2 && y <= cy + h/2) {
                return sectors[i].name;
            }
        }
        return nullptr;
    }
    
    // Get sector dimensions (for position calculation)
    bool getSectorDimensions(const char* name, float& width, float& height) {
        char normalizedInput[100];
        normalizeSectorName(name, normalizedInput);
        
        for (int i = 0; i < sectorCount; i++) {
            char normalizedSector[100];
            normalizeSectorName(sectors[i].name, normalizedSector);
            
            if (strcmp(normalizedSector, normalizedInput) == 0) {
                width = sectors[i].width;
                height = sectors[i].height;
                return true;
            }
        }
        width = DEFAULT_CELL_WIDTH;
        height = DEFAULT_CELL_HEIGHT;
        return false;
    }
    
    int getSectorCount() { return sectorCount; }
    Sector* getSector(int idx) { return &sectors[idx]; }
    
    // Get total map dimensions (dynamic based on actual sector sizes)
    float getMapWidth() { 
        float maxX = 0;
        for (int i = 0; i < sectorCount; i++) {
            float rightEdge = sectors[i].centerX + sectors[i].width / 2;
            if (rightEdge > maxX) maxX = rightEdge;
        }
        return maxX + GRID_START_X; 
    }
    float getMapHeight() { 
        float maxY = 0;
        for (int i = 0; i < sectorCount; i++) {
            float bottomEdge = sectors[i].centerY + sectors[i].height / 2;
            if (bottomEdge > maxY) maxY = bottomEdge;
        }
        return maxY + GRID_START_Y; 
    }
};

#endif
