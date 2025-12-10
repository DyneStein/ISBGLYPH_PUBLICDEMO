#ifndef CITYRENDERER_H
#define CITYRENDERER_H

#include <SFML/Graphics.hpp>
#include <cstdio>

class CityRenderer {
private:
    // Textures
    sf::Texture backgroundTex;
    sf::Texture busStopTex;
    sf::Texture hospitalTex;
    sf::Texture schoolTex;
    sf::Texture pharmacyTex;
    sf::Texture mosqueTex;
    sf::Texture bankTex;
    sf::Texture hotelTex;
    sf::Texture mallTex;
    sf::Texture busTex;
    sf::Texture trainTex;
    sf::Texture aeroplaneTex;
    sf::Texture parkTex;
    sf::Texture airportTex;
    sf::Texture trainStationTex;
    
    // Decorative textures (not linked to any system)
    sf::Texture houseTex;
    sf::Texture flatsTex;
    sf::Texture factoryTex;
    sf::Texture waterTowerTex;
    sf::Texture restaurantTex;
    sf::Texture gasStationTex;
    sf::Texture policeStationTex;
    sf::Texture fireStationTex;
    
    bool texturesLoaded;
    
public:
    CityRenderer() : texturesLoaded(false) {}
    
    bool loadTextures(const char* assetPath) {
        char path[256];
        
        // Load background
        snprintf(path, 256, "%s/sprites/background.png", assetPath);
        if (!backgroundTex.loadFromFile(path)) {
            printf("[WARN] Could not load background.png\n");
        }
        
        // Load building sprites
        snprintf(path, 256, "%s/sprites/T_busstop.png", assetPath);
        if (!busStopTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_hospital.png", assetPath);
        if (!hospitalTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_school.png", assetPath);
        if (!schoolTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_Pharmacy.png", assetPath);
        if (!pharmacyTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_Mosque.png", assetPath);
        if (!mosqueTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_Bank.png", assetPath);
        if (!bankTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_hotel.png", assetPath);
        if (!hotelTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_Mall.png", assetPath);
        if (!mallTex.loadFromFile(path)) return false;
        
        // Load vehicle sprites
        snprintf(path, 256, "%s/sprites/T_bus.png", assetPath);
        if (!busTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_train.png", assetPath);
        if (!trainTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_aeroplane.png", assetPath);
        if (!aeroplaneTex.loadFromFile(path)) return false;
        
        // Load facility sprites  
        snprintf(path, 256, "%s/sprites/T_park.png", assetPath);
        if (!parkTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_airport.png", assetPath);
        if (!airportTex.loadFromFile(path)) return false;
        
        snprintf(path, 256, "%s/sprites/T_trainStation.png", assetPath);
        if (!trainStationTex.loadFromFile(path)) return false;
        
        // Load decorative sprites (not linked to systems)
        snprintf(path, 256, "%s/sprites/T_house.png", assetPath);
        if (!houseTex.loadFromFile(path)) printf("[WARN] Could not load T_house.png\n");
        
        snprintf(path, 256, "%s/sprites/T_Flats.png", assetPath);
        if (!flatsTex.loadFromFile(path)) printf("[WARN] Could not load T_Flats.png\n");
        
        snprintf(path, 256, "%s/sprites/T_Factory.png", assetPath);
        if (!factoryTex.loadFromFile(path)) printf("[WARN] Could not load T_Factory.png\n");
        
        snprintf(path, 256, "%s/sprites/T_WaterTower.png", assetPath);
        if (!waterTowerTex.loadFromFile(path)) printf("[WARN] Could not load T_WaterTower.png\n");
        
        snprintf(path, 256, "%s/sprites/T_restaurant.png", assetPath);
        if (!restaurantTex.loadFromFile(path)) printf("[WARN] Could not load T_restaurant.png\n");
        
        snprintf(path, 256, "%s/sprites/T_gasStation.png", assetPath);
        if (!gasStationTex.loadFromFile(path)) printf("[WARN] Could not load T_gasStation.png\n");
        
        snprintf(path, 256, "%s/sprites/T_policeStation.png", assetPath);
        if (!policeStationTex.loadFromFile(path)) printf("[WARN] Could not load T_policeStation.png\n");
        
        snprintf(path, 256, "%s/sprites/T_fireStation.png", assetPath);
        if (!fireStationTex.loadFromFile(path)) printf("[WARN] Could not load T_fireStation.png\n");
        
        texturesLoaded = true;
        return true;
    }
    
    // Helper to draw a sprite centered at position with given size
    void drawSprite(sf::RenderWindow& window, sf::Texture& tex, float x, float y, float size, bool highlight = false) {
        sf::Sprite sprite(tex);
        float texWidth = (float)tex.getSize().x;
        float texHeight = (float)tex.getSize().y;
        float scale = size / texWidth;
        
        // Calculate actual scaled dimensions
        float scaledWidth = size;
        float scaledHeight = texHeight * scale;
        
        sprite.setScale(sf::Vector2f(scale, scale));
        // Center based on actual scaled dimensions
        sprite.setPosition(sf::Vector2f(x - scaledWidth/2, y - scaledHeight/2));
        if (highlight) {
            sprite.setColor(sf::Color(255, 255, 150));
        }
        window.draw(sprite);
    }
    
    // Draw background tiles - optimized with view culling and sprite reuse
    void drawBackground(sf::RenderWindow& window, float mapWidth, float mapHeight) {
        // Get the current view bounds for culling
        sf::View view = window.getView();
        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewSize = view.getSize();
        
        // Calculate visible area bounds with buffer
        float viewLeft = viewCenter.x - viewSize.x / 2 - 500;
        float viewRight = viewCenter.x + viewSize.x / 2 + 500;
        float viewTop = viewCenter.y - viewSize.y / 2 - 500;
        float viewBottom = viewCenter.y + viewSize.y / 2 + 500;
        
        // Green base - only draw if visible
        sf::RectangleShape base(sf::Vector2f(15000.0f, 15000.0f));
        base.setPosition(sf::Vector2f(-4000, -4000));
        base.setFillColor(sf::Color(55, 95, 55));
        window.draw(base);
        
        // Tile background texture - only visible tiles
        if (backgroundTex.getSize().x > 0 && backgroundTex.getSize().y > 0) {
            float texWidth = (float)backgroundTex.getSize().x;
            float texHeight = (float)backgroundTex.getSize().y;
            
            float targetWidth = 900.0f;
            float scaleX = targetWidth / texWidth;
            float scaleY = scaleX;
            
            float tileWidth = texWidth * scaleX;
            float tileHeight = texHeight * scaleY;
            float stepX = tileWidth - 10.0f;
            float stepY = tileHeight - 10.0f;
            
            // Only iterate over tiles that could be visible
            float startX = std::floor((viewLeft + 4000) / stepX) * stepX - 4000;
            float startY = std::floor((viewTop + 4000) / stepY) * stepY - 4000;
            float endX = viewRight;
            float endY = viewBottom;
            
            // Create sprite ONCE outside the loop and reuse it
            sf::Sprite tile(backgroundTex);
            tile.setScale(sf::Vector2f(scaleX, scaleY));
            
            for (float y = startY; y < endY; y += stepY) {
                for (float x = startX; x < endX; x += stepX) {
                    tile.setPosition(sf::Vector2f(x, y));
                    window.draw(tile);
                }
            }
        }
    }
    
    // Draw connection line between two points
    void drawConnection(sf::RenderWindow& window, float x1, float y1, float x2, float y2, bool highlight = false) {
        sf::Color color = highlight ? sf::Color(100, 255, 150) : sf::Color(255, 220, 80, 180);
        float thickness = highlight ? 6.0f : 4.0f;
        
        // Calculate line vertices
        float dx = x2 - x1;
        float dy = y2 - y1;
        float len = std::sqrt(dx*dx + dy*dy);
        if (len < 1) return;
        
        float nx = -dy / len * thickness / 2;
        float ny = dx / len * thickness / 2;
        
        sf::ConvexShape line(4);
        line.setPoint(0, sf::Vector2f(x1 + nx, y1 + ny));
        line.setPoint(1, sf::Vector2f(x1 - nx, y1 - ny));
        line.setPoint(2, sf::Vector2f(x2 - nx, y2 - ny));
        line.setPoint(3, sf::Vector2f(x2 + nx, y2 + ny));
        line.setFillColor(color);
        window.draw(line);
    }
    
    // Airport connections - Orange color
    void drawAirportConnection(sf::RenderWindow& window, float x1, float y1, float x2, float y2) {
        sf::Color color = sf::Color(255, 140, 0, 200);  // Orange
        float thickness = 5.0f;
        
        float dx = x2 - x1;
        float dy = y2 - y1;
        float len = std::sqrt(dx*dx + dy*dy);
        if (len < 1) return;
        
        float nx = -dy / len * thickness / 2;
        float ny = dx / len * thickness / 2;
        
        sf::ConvexShape line(4);
        line.setPoint(0, sf::Vector2f(x1 + nx, y1 + ny));
        line.setPoint(1, sf::Vector2f(x1 - nx, y1 - ny));
        line.setPoint(2, sf::Vector2f(x2 - nx, y2 - ny));
        line.setPoint(3, sf::Vector2f(x2 + nx, y2 + ny));
        line.setFillColor(color);
        window.draw(line);
    }
    
    // Railway connections - Purple color
    void drawRailwayConnection(sf::RenderWindow& window, float x1, float y1, float x2, float y2) {
        sf::Color color = sf::Color(147, 51, 234, 200);  // Purple
        float thickness = 5.0f;
        
        float dx = x2 - x1;
        float dy = y2 - y1;
        float len = std::sqrt(dx*dx + dy*dy);
        if (len < 1) return;
        
        float nx = -dy / len * thickness / 2;
        float ny = dx / len * thickness / 2;
        
        sf::ConvexShape line(4);
        line.setPoint(0, sf::Vector2f(x1 + nx, y1 + ny));
        line.setPoint(1, sf::Vector2f(x1 - nx, y1 - ny));
        line.setPoint(2, sf::Vector2f(x2 - nx, y2 - ny));
        line.setPoint(3, sf::Vector2f(x2 + nx, y2 + ny));
        line.setFillColor(color);
        window.draw(line);
    }
    
    // Entity drawing functions
    void drawBusStop(sf::RenderWindow& window, float x, float y, bool selected = false) {
        drawSprite(window, busStopTex, x, y, 150.0f, selected);
    }
    
    void drawHospital(sf::RenderWindow& window, float x, float y) {
        drawSprite(window, hospitalTex, x, y, 280.0f);
    }
    
    void drawSchool(sf::RenderWindow& window, float x, float y) {
        drawSprite(window, schoolTex, x, y, 220.0f);
    }
    
    void drawPharmacy(sf::RenderWindow& window, float x, float y) {
        drawSprite(window, pharmacyTex, x, y, 180.0f);
    }
    
    void drawMosque(sf::RenderWindow& window, float x, float y) {
        drawSprite(window, mosqueTex, x, y, 225.0f);  // Reduced by 25% from 300
    }
    
    void drawBank(sf::RenderWindow& window, float x, float y) {
        drawSprite(window, bankTex, x, y, 200.0f);
    }
    
    void drawHotel(sf::RenderWindow& window, float x, float y) {
        drawSprite(window, hotelTex, x, y, 250.0f);
    }
    
    void drawMall(sf::RenderWindow& window, float x, float y) {
        drawSprite(window, mallTex, x, y, 280.0f);
    }
    
    void drawPark(sf::RenderWindow& window, float x, float y) {
        // Parks use dedicated park texture
        drawSprite(window, parkTex, x, y, 375.0f);  // Increased by 50% from 250
    }
    
    void drawAirport(sf::RenderWindow& window, float x, float y) {
        // Airports use dedicated airport texture - draw at stored position
        drawSprite(window, airportTex, x, y, 260.0f);
    }
    
    void drawTrainStation(sf::RenderWindow& window, float x, float y) {
        // Train stations use dedicated train station texture - draw at stored position
        drawSprite(window, trainStationTex, x, y, 220.0f);
    }
    
    void drawBus(sf::RenderWindow& window, float x, float y) {
        // Buses use dedicated bus texture
        drawSprite(window, busTex, x, y, 144.0f);  // Increased by 20% from 120
    }
    
    void drawTrain(sf::RenderWindow& window, float x, float y) {
        // Trains use dedicated train texture
        drawSprite(window, trainTex, x, y, 115.0f);  // Reduced from 150
    }
    
    void drawAirplane(sf::RenderWindow& window, float x, float y) {
        // Airplanes use dedicated aeroplane texture
        drawSprite(window, aeroplaneTex, x, y, 110.0f);  // Reduced from 130
    }
    
    // ===== DECORATIVE DRAW FUNCTIONS =====
    void drawDecoration(sf::RenderWindow& window, int type, float x, float y) {
        switch (type) {
            case 0: drawSprite(window, houseTex, x, y, 140.0f); break;        // House
            case 1: drawSprite(window, flatsTex, x, y, 180.0f); break;        // Flats
            case 2: drawSprite(window, factoryTex, x, y, 200.0f); break;      // Factory
            case 3: drawSprite(window, waterTowerTex, x, y, 120.0f); break;   // Water Tower
            case 4: drawSprite(window, restaurantTex, x, y, 160.0f); break;   // Restaurant
            case 5: drawSprite(window, gasStationTex, x, y, 180.0f); break;   // Gas Station
            case 6: drawSprite(window, policeStationTex, x, y, 180.0f); break;// Police Station
            case 7: drawSprite(window, fireStationTex, x, y, 180.0f); break;  // Fire Station
            default: break;
        }
    }
    // Helper for batch rendering
    const sf::Texture* getDecorationTexture(int type) const {
        switch(type) {
            case 0: return &houseTex;
            case 1: return &flatsTex;
            case 2: return &factoryTex;
            case 3: return &waterTowerTex;
            case 4: return &restaurantTex;
            case 5: return &gasStationTex;
            case 6: return &policeStationTex;
            case 7: return &fireStationTex;
            default: return &houseTex;
        }
    }
    
    // Getters for building textures (for similar optimizations if needed)
    const sf::Texture* getHospitalTexture() const { return &hospitalTex; }
    const sf::Texture* getSchoolTexture() const { return &schoolTex; }
    const sf::Texture* getPharmacyTexture() const { return &pharmacyTex; }
    const sf::Texture* getMallTexture() const { return &mallTex; }
    const sf::Texture* getParkTexture() const { return &parkTex; }
    const sf::Texture* getMosqueTexture() const { return &mosqueTex; }
    const sf::Texture* getAirportTexture() const { return &airportTex; }
    const sf::Texture* getTrainStationTexture() const { return &trainStationTex; }
    const sf::Texture* getBusStopTexture() const { return &busStopTex; }
};

#endif
