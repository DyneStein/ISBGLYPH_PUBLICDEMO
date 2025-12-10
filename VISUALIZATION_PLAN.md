# Smart City Visualization - Complete Restart Plan

## Overview
A clean SFML 3 visualization for the Islamabad Smart City simulation. Uses a **simple grid-based isometric layout** where each sector is a cell, and entities are placed within their sector cells.

---

## Core Architecture

### 1. Coordinate System (Simple Grid)
```
SECTOR GRID (7 sectors arranged in 2 rows):
Row 1: G-10 | F-10 | G-9  | F-8
Row 2:      | F-7  | Blue | F-6

Each cell: 600x500 pixels
Grid starts at (300, 200) with 650px horizontal, 550px vertical spacing
```

### 2. File Structure
```
include/
├── Visualization.h    # Main game class (simple, clean)
├── CityRenderer.h     # All sprite drawing functions
├── CityMap.h          # Grid layout and sector positions
└── (existing data systems unchanged)
```

---

## Sector Grid Positions
| Sector | Row | Col | Center X | Center Y |
|--------|-----|-----|----------|----------|
| G-10 | 0 | 0 | 600 | 450 |
| F-10 | 0 | 1 | 1250 | 450 |
| G-9 | 0 | 2 | 1900 | 450 |
| F-8 | 0 | 3 | 2550 | 450 |
| F-7 | 1 | 1 | 1250 | 1000 |
| Blue Area | 1 | 2 | 1900 | 1000 |
| F-6 | 1 | 3 | 2550 | 1000 |

---

## Rendering Order (Layers)
1. **Background** - Large tiled grass texture
2. **Sector zones** - Colored rectangles with borders
3. **Roads/Connections** - Yellow lines between stops
4. **Buildings** - Hospitals, schools, pharmacies (back)
5. **Bus stops** - Transport nodes
6. **Labels** - Sector names (TOP layer, never hidden)
7. **UI Panel** - Fixed left sidebar (reset view before drawing)

---

## Entity Placement (Within Sectors)
Each sector cell (600x500) has a placement grid:
```
+--------+--------+--------+
| Label  |  Med   | School |
+--------+--------+--------+
| Build  | STOP   | Build  |
+--------+--------+--------+
| Deco   | Deco   | Deco   |
+--------+--------+--------+
```

**Offset positions from sector center:**
- **Label**: (-280, -230)
- **Bus Stop**: (0, 0) - center
- **Hospital**: (+150, -150)
- **School**: (+150, +100)
- **Pharmacy**: (-150, +100)
- **Decorative**: spread at bottom

---

## Implementation Steps

### Phase 1: Clean Foundation (Day 1)
- [ ] Delete existing Game.h, GameMap.h, Renderer.h, InputHandler.h
- [ ] Create new CityMap.h with simple grid positions
- [ ] Create new CityRenderer.h with sprite loading only
- [ ] Create new Visualization.h with basic game loop

### Phase 2: Core Rendering (Day 1)
- [ ] Draw background texture (tiled, overlapping)
- [ ] Draw sector zones as colored rectangles
- [ ] Draw sector labels (on TOP of everything)
- [ ] Implement camera with WASD (using sf::View)
- [ ] Implement zoom with Q/E

### Phase 3: Data Integration (Day 2)
- [ ] Load bus stops from CSV - place at sector centers
- [ ] Load hospitals from CSV - place offset in sectors
- [ ] Load schools from CSV - place offset in sectors
- [ ] Load pharmacies from CSV - place offset in sectors
- [ ] Draw connections between stops

### Phase 4: Interactivity (Day 2)
- [ ] Click to select entities
- [ ] Show info popup on selection
- [ ] Highlight connections for selected stop
- [ ] Display current sector in UI

### Phase 5: Polish
- [ ] Add toggle buttons for layers (1-4 keys)
- [ ] Animate selected entities
- [ ] Show shortest path on demand

---

## Key Design Decisions

1. **Grid-based layout** - No complex lat/lon conversion, just fixed grid cells
2. **Separate label layer** - Labels drawn AFTER all entities
3. **Overlapping background tiles** - Prevents visible gaps
4. **View-based zoom** - Camera handled by sf::View, not manual offsets
5. **Data-driven** - Positions derived from sector grid, not hardcoded

---

## Files to Delete
- include/Game.h
- include/GameMap.h
- include/Renderer.h
- include/InputHandler.h
- include/UI.h

## Files to Create
- include/Visualization.h - Main game class
- include/CityMap.h - Sector grid and positions
- include/CityRenderer.h - Sprite loading and drawing
