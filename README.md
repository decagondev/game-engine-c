# Coin Collector Game Engine

A 2D/3D game engine built with Raylib featuring a coin collection game with both top-down 2D and first-person 3D raycast rendering modes.

## Features

### Dual Rendering Modes

#### 2D Mode (Top-Down View)
- Classic top-down perspective with full map visibility
- Complete game mechanics including:
  - Projectile shooting system
  - Enemy collision detection
  - Coin collection
  - Map transitions
  - Health system

#### 3D Mode (First-Person Raycast)
- Real-time raycast rendering engine
- First-person perspective with immersive gameplay
- Advanced visual features:
  - **Wall Rendering**: Distance-based shading with directional lighting
  - **Enemy Sprites**: 3D sprite rendering with depth sorting
  - **Coin Sprites**: Golden coin rendering with visibility culling
  - **Gradient Floor/Ceiling**: Smooth color transitions
  - **Minimap Overlay**: Real-time map view with player position
  - **Wall Side Detection**: Different shading for north/south/east/west walls

### Game Mechanics

- **Coin Collection**: Collect all coins across 4 interconnected maps
- **Enemy Avoidance**: Red obstacles move around and damage the player
- **Health System**: Player has 100 HP, takes 10 damage per enemy hit
- **Invincibility Frames**: 60 frames of invincibility after taking damage
- **Map System**: 4 different maps with exits and entrances
- **High Score System**: Track best completion times
- **Projectile System**: (2D mode only) Shoot projectiles with mouse or keyboard

### Controls

#### 2D Mode
- **WASD** or **Arrow Keys**: Move player
- **Mouse Click** or **SPACE + Direction**: Shoot projectiles
- **ESC**: Quit game

#### 3D Mode
- **WASD** or **Arrow Keys**: Move forward/backward and strafe
- **Mouse Drag** (Left/Right Button): Look around (smooth rotation)
- **Q/E Keys**: Rotate left/right (alternative to mouse)
- **ESC**: Quit game

#### Menu Navigation
- **SPACE** or **ENTER**: Start game / Select option
- **UP/DOWN** or **W/S**: Navigate menu options
- **H**: View high scores
- **ESC**: Go back / Quit

## Building

### Prerequisites
- CMake 3.10 or higher
- C compiler (GCC, Clang, or MSVC)
- Raylib (automatically downloaded via CMake FetchContent)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run
./bin/GameEngine
```

On Windows:
```bash
cmake --build . --config Release
.\bin\GameEngine.exe
```

## Project Structure

```
3drender/
├── include/           # Header files
│   ├── audio.h       # Audio system
│   ├── enemy.h       # Enemy/obstacle logic
│   ├── game.h        # Main game structure
│   ├── gengine.h     # Game engine core
│   ├── highscore.h   # High score management
│   ├── item.h        # Item system (coins)
│   ├── map.h         # Map and level data
│   ├── player.h      # Player logic
│   ├── projectile.h  # Projectile system
│   ├── raycaster.h   # 3D raycasting engine
│   ├── renderer.h    # 2D renderer
│   ├── renderer3d.h  # 3D renderer
│   └── state.h       # Game state management
├── src/              # Source files
│   ├── audio.c
│   ├── enemy.c
│   ├── game.c
│   ├── gengine.c
│   ├── highscore.c
│   ├── item.c
│   ├── main.c
│   ├── map.c
│   ├── player.c
│   ├── projectile.c
│   ├── raycaster.c
│   ├── renderer.c
│   ├── renderer3d.c
│   └── state.c
├── CMakeLists.txt    # Build configuration
└── README.md         # This file
```

## Technical Details

### 3D Rendering Engine

The 3D mode uses a custom raycasting engine similar to classic games like Wolfenstein 3D:

- **Field of View**: 60 degrees
- **Raycasting Algorithm**: Line-rectangle intersection for wall detection
- **Perspective Correction**: Perpendicular distance calculation to avoid fisheye effect
- **Wall Shading**: 
  - Distance-based darkening
  - Side-based lighting (simulates light from above)
  - Minimum brightness to maintain visibility
- **Sprite Rendering**:
  - Enemies and coins rendered as 2D sprites
  - Depth sorting for proper occlusion
  - Distance-based scaling and shading
  - FOV culling and wall occlusion

### Game State Management

The engine uses a modular state system:
- **State Module**: Centralized game state management
- **High Score Module**: Persistent high score tracking
- **Map Module**: Level data and collision detection
- **Player Module**: Player logic with health and movement
- **Enemy Module**: Enemy AI and movement
- **Projectile Module**: Projectile physics and collision

### Architecture

- **Modular Design**: Separated concerns with dedicated modules
- **Callback System**: Game engine uses callbacks for initialization, update, render, and cleanup
- **Memory Management**: Proper allocation and cleanup to prevent leaks
- **Collision Detection**: Circle-rectangle and circle-circle collision systems

## Gameplay

### Objective
Collect all coins across all 4 maps as quickly as possible while avoiding enemies.

### Strategy
- Plan your route through the maps
- Avoid red obstacles (enemies)
- Use exits (green rectangles) to move between maps
- In 2D mode, use projectiles to clear enemies
- In 3D mode, use the minimap to navigate

### Scoring
Your score is based on the number of frames it takes to complete the game. Lower frame counts = better scores!

## Future Enhancements

Potential improvements:
- Texture mapping for walls in 3D mode
- More sophisticated enemy AI
- Additional maps
- Power-ups and special items
- Sound effects and music
- Particle effects
- Improved sprite graphics

## License

This project uses Raylib, which is licensed under zlib/libpng license.

## Credits

Built with:
- [Raylib](https://www.raylib.com/) - Simple and easy-to-use library for game development
- CMake - Cross-platform build system
