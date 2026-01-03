# Game Engine

A lightweight, modular game engine built in C using Raylib. This engine provides a clean, callback-based architecture that makes it easy to create 2D games while maintaining separation of concerns and following SOLID principles.

## Features

- **Modular Architecture**: Clean separation between engine, game logic, rendering, audio, and game entities
- **Callback-Based Design**: Simple callback system for game initialization, updates, rendering, and cleanup
- **Cross-Platform**: Works on Windows, macOS, and Linux
- **Built-in Modules**: 
  - Audio system with procedural sound generation
  - Map system with collision detection
  - Renderer for drawing game elements
  - Player, Enemy, and Item entity systems
  - Projectile system for combat mechanics
  - State management system
  - High score system with persistent storage
- **SOLID Principles**: Well-structured codebase following best practices
- **Extensible**: Easy to add new features and game types

## Demo Game: Coin Collector

The engine includes a complete demo game where players collect coins across 4 interconnected maps while avoiding obstacles. Features include:

- WASD movement controls
- Mouse click or SPACE+Arrow key shooting with projectiles
- Multiple interconnected maps with exits/entrances
- Collectible coins
- Moving obstacles that damage the player
- Health system with invincibility frames
- High score system with persistent storage
- Multiple game states (start, playing, end, name entry, high scores)

## Building

### Prerequisites

- CMake 3.16 or higher
- C compiler (GCC, Clang, or MSVC)
- Git (for fetching dependencies)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run the demo game
./bin/GameEngine
```

On Windows:
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\bin\Release\GameEngine.exe
```

## Architecture

The engine follows a modular architecture with clear separation of concerns:

```
Game Engine
├── Engine Core (gengine)
│   ├── Window management
│   ├── Main loop
│   └── Callback system
├── Game Logic (game)
│   ├── Game callbacks
│   └── Game-specific logic
├── State Management (state)
│   ├── Game state management
│   ├── Projectile management
│   └── State transitions
├── High Score System (highscore)
│   ├── Score loading/saving
│   └── Score ranking
├── Rendering (renderer)
│   ├── Map rendering
│   ├── Entity rendering
│   ├── Projectile rendering
│   └── UI rendering
├── Audio (audio)
│   └── Procedural sound generation
├── Map System (map)
│   ├── Collision detection
│   └── Map data structures
├── Entities
│   ├── Player
│   ├── Enemy
│   ├── Item
│   └── Projectile
```

## API Documentation

### Engine Core API

#### `gengine.h`

The engine core provides the main game loop and callback system.

**Engine Configuration:**
```c
typedef struct {
    int screen_width;        // Window width in pixels
    int screen_height;       // Window height in pixels
    const char* window_title; // Window title string
    int target_fps;          // Target frames per second
} EngineConfig;
```

**Game Callbacks:**
```c
typedef struct {
    void (*init)(void* game_data);           // Called once at startup
    void (*update)(void* game_data, float delta_time); // Called every frame
    void (*render)(void* game_data);         // Called every frame after update
    void (*cleanup)(void* game_data);        // Called on shutdown
    void (*handle_input)(void* game_data, int key); // Optional input handler
} GameCallbacks;
```

**Functions:**
- `GameEngine* gengine_create(const EngineConfig* config)` - Create engine instance
- `void gengine_destroy(GameEngine* engine)` - Destroy engine instance
- `void gengine_register_game(GameEngine* engine, GameCallbacks* callbacks, void* game_data)` - Register your game
- `void gengine_run(GameEngine* engine)` - Start the main loop
- `int gengine_get_frame_count(GameEngine* engine)` - Get current frame count
- `bool gengine_is_running(GameEngine* engine)` - Check if engine is running
- `void gengine_stop(GameEngine* engine)` - Request engine to stop

### State Management API

#### `state.h`

Centralized game state management system.

**Game State Types:**
```c
typedef enum {
    GAME_STATE_START,
    GAME_STATE_PLAYING,
    GAME_STATE_END,
    GAME_STATE_ENTER_NAME,
    GAME_STATE_HIGH_SCORES
} GameStateType;
```

**State Creation/Destruction:**
- `GameState* state_create(void)` - Create new game state
- `void state_destroy(GameState* state)` - Destroy game state
- `void state_init(GameState* state)` - Initialize game state
- `void state_reset(GameState* state)` - Reset to initial playing state

**State Queries:**
- `GameStateType state_get_type(const GameState* state)` - Get current state type
- `bool state_is_running(const GameState* state)` - Check if running
- `int state_get_frame_count(const GameState* state)` - Get frame count
- `Player* state_get_player(GameState* state)` - Get player instance
- `Map* state_get_current_map(GameState* state)` - Get current map
- `int state_get_coins_collected(const GameState* state)` - Get coins collected
- `bool state_all_coins_collected(const GameState* state)` - Check if all coins collected

**State Modifications:**
- `void state_set_type(GameState* state, GameStateType type)` - Set state type
- `void state_set_running(GameState* state, bool running)` - Set running flag
- `void state_increment_coins_collected(GameState* state)` - Increment coin count

**Projectile Management:**
- `bool state_add_projectile(GameState* state, Projectile* projectile)` - Add projectile
- `Projectile** state_get_projectiles(GameState* state, int* count)` - Get all projectiles
- `void state_remove_projectile(GameState* state, int index)` - Remove projectile
- `void state_clear_projectiles(GameState* state)` - Clear all projectiles
- `int state_get_projectile_cooldown(const GameState* state)` - Get cooldown
- `void state_set_projectile_cooldown(GameState* state, int cooldown)` - Set cooldown

### High Score API

#### `highscore.h`

High score management with persistent file storage.

**High Score Structure:**
```c
typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int frame_count;
    int coins_collected;
    float health_remaining;
} HighScore;
```

**Functions:**
- `void highscore_load(HighScore* high_scores, int* count)` - Load from file
- `void highscore_save(const HighScore* high_scores, int count)` - Save to file
- `bool highscore_add(HighScore* high_scores, int* count, const char* name, int frame_count, int coins_collected, float health_remaining)` - Add new high score

### Projectile API

#### `projectile.h`

Projectile system for combat mechanics.

**Creation/Destruction:**
- `Projectile* projectile_create(Vector2 position, Vector2 direction)` - Create projectile
- `void projectile_destroy(Projectile* projectile)` - Destroy projectile

**State Queries:**
- `Vector2 projectile_get_position(const Projectile* projectile)` - Get position
- `Vector2 projectile_get_velocity(const Projectile* projectile)` - Get velocity
- `float projectile_get_radius(const Projectile* projectile)` - Get radius
- `float projectile_get_damage(const Projectile* projectile)` - Get damage
- `bool projectile_is_active(const Projectile* projectile)` - Check if active

**Update Logic:**
- `void projectile_update(Projectile* projectile)` - Update position and lifetime

**Collision Detection:**
- `bool projectile_check_circle_collision(const Projectile* projectile, Vector2 circle_pos, float circle_radius)` - Check circle collision
- `bool projectile_check_rect_collision(const Projectile* projectile, Rectangle rect)` - Check rectangle collision

**Constants:**
- `PROJECTILE_SPEED = 10.0f`
- `PROJECTILE_RADIUS = 5.0f`
- `PROJECTILE_DAMAGE = 20.0f`
- `PROJECTILE_LIFETIME = 120` (frames)

### Audio API

#### `audio.h`

Procedural audio system for generating simple sound effects.

**Sound Types:**
```c
typedef enum {
    AUDIO_SOUND_COIN,      // Coin collection sound
    AUDIO_SOUND_DAMAGE,    // Damage/hit sound
    AUDIO_SOUND_VICTORY,   // Victory/completion sound
    AUDIO_SOUND_MENU       // Menu interaction sound
} AudioSoundType;
```

**Functions:**
- `bool audio_init(void)` - Initialize audio system
- `void audio_cleanup(void)` - Cleanup audio system
- `void audio_play_sound(AudioSoundType sound_type)` - Play predefined sound
- `void audio_play_blip(float frequency, float duration, float volume)` - Play custom blip sound

### Map System API

#### `map.h`

Map system for managing game levels with walls, exits, entrances, coins, and obstacles.

**Map Creation:**
- `Map* map_create(int map_id)` - Create a new map
- `void map_destroy(Map* map)` - Destroy a map
- `void map_init(Map* map, int map_id)` - Initialize a map with specific ID

**Collision Detection:**
- `bool map_check_circle_rect_collision(Vector2 circle_pos, float radius, Rectangle rect)` - Check circle-rectangle collision
- `bool map_is_valid_spawn_position(Vector2 position, float radius, const Map* map)` - Check if position is valid for spawning
- `Vector2 map_find_valid_spawn_position(Vector2 desired_pos, float radius, const Map* map)` - Find valid spawn position near desired location

**Map Queries:**
- `const Wall* map_get_walls(const Map* map, int* count)` - Get walls array
- `const Exit* map_get_exits(const Map* map, int* count)` - Get exits array
- `const Entrance* map_get_entrances(const Map* map, int* count)` - Get entrances array
- `const Coin* map_get_coins(const Map* map, int* count)` - Get coins array
- `const Obstacle* map_get_obstacles(const Map* map, int* count)` - Get obstacles array
- `Color map_get_background_color(const Map* map)` - Get map background color
- `int map_get_id(const Map* map)` - Get map ID

**Map Modifications:**
- `Coin* map_get_coin_mutable(Map* map, int index)` - Get mutable coin reference
- `Obstacle* map_get_obstacle_mutable(Map* map, int index)` - Get mutable obstacle reference
- `void map_update_obstacle(Map* map, int index, Vector2 new_position, Vector2 new_velocity, int new_timer)` - Update obstacle state

### Renderer API

#### `renderer.h`

Rendering system for drawing game elements and UI.

**Initialization:**
- `void renderer_init(void)` - Initialize renderer
- `void renderer_clear(Color color)` - Clear screen with color

**Map Rendering:**
- `void renderer_draw_map(const Map* map)` - Draw map (walls, exits)

**Entity Rendering:**
- `void renderer_draw_coin(Vector2 position, bool collected)` - Draw coin
- `void renderer_draw_obstacle(Vector2 position, float radius, Color color)` - Draw obstacle
- `void renderer_draw_player(Vector2 position, bool invincible, int invincibility_timer)` - Draw player
- `void renderer_draw_projectile(Vector2 position, float radius)` - Draw projectile

**UI Rendering:**
- `void renderer_draw_health_bar(float x, float y, float width, float height, float health, float max_health)` - Draw health bar
- `void renderer_draw_text_centered(const char* text, int y, int font_size, Color color)` - Draw centered text
- `void renderer_draw_text(const char* text, int x, int y, int font_size, Color color)` - Draw text
- `void renderer_draw_fps(int x, int y)` - Draw FPS counter

**Screen Rendering:**
- `void renderer_draw_start_screen(int frame_count, const HighScore* high_scores, int high_score_count)` - Draw start screen
- `void renderer_draw_end_screen(...)` - Draw end/victory screen
- `void renderer_draw_name_entry_screen(...)` - Draw name entry screen
- `void renderer_draw_high_scores_screen(...)` - Draw high scores screen
- `void renderer_draw_game_screen(..., Projectile** projectiles, int projectile_count)` - Draw main game screen

### Player API

#### `player.h`

Player entity system for managing player state, movement, and collisions.

**Creation/Destruction:**
- `Player* player_create(void)` - Create player instance
- `void player_destroy(Player* player)` - Destroy player instance
- `void player_init(Player* player, Vector2 start_position)` - Initialize player

**State Queries:**
- `Vector2 player_get_position(const Player* player)` - Get position
- `float player_get_health(const Player* player)` - Get health
- `float player_get_max_health(const Player* player)` - Get max health
- `bool player_is_invincible(const Player* player)` - Check invincibility
- `int player_get_invincibility_timer(const Player* player)` - Get invincibility timer
- `bool player_is_alive(const Player* player)` - Check if alive

**State Modifications:**
- `void player_set_position(Player* player, Vector2 position)` - Set position
- `void player_set_health(Player* player, float health)` - Set health
- `void player_reset(Player* player, Vector2 start_position)` - Reset player

**Movement:**
- `void player_update_movement(Player* player, const struct Map* current_map)` - Update movement based on input
- `void player_handle_input(Player* player)` - Handle input (placeholder)

**Collision Detection:**
- `bool player_check_wall_collision(const Player* player, Vector2 new_position, const struct Map* current_map)` - Check wall collision
- `bool player_check_exit_collision(const Player* player, const struct Map* current_map, int* target_map_id, int* target_entrance_id)` - Check exit collision
- `void player_apply_damage(Player* player, float damage)` - Apply damage
- `void player_update(Player* player)` - Update player state (invincibility timer)

### Enemy API

#### `enemy.h`

Enemy entity system for managing enemy behavior and movement.

**Creation/Destruction:**
- `Enemy* enemy_create(Vector2 position, Vector2 velocity, Color color)` - Create enemy instance
- `void enemy_destroy(Enemy* enemy)` - Destroy enemy instance

**State Queries:**
- `Vector2 enemy_get_position(const Enemy* enemy)` - Get position
- `Vector2 enemy_get_velocity(const Enemy* enemy)` - Get velocity
- `float enemy_get_radius(const Enemy* enemy)` - Get radius
- `Color enemy_get_color(const Enemy* enemy)` - Get color
- `int enemy_get_direction_timer(const Enemy* enemy)` - Get direction change timer

**State Modifications:**
- `void enemy_set_position(Enemy* enemy, Vector2 position)` - Set position
- `void enemy_set_velocity(Enemy* enemy, Vector2 velocity)` - Set velocity
- `void enemy_set_direction_timer(Enemy* enemy, int timer)` - Set direction timer

**Update Logic:**
- `void enemy_update(Enemy* enemy, const struct Map* current_map)` - Update enemy state and movement

**Collision Detection:**
- `bool enemy_check_collision_with_player(const Enemy* enemy, Vector2 player_position, float player_radius)` - Check player collision
- `bool enemy_check_wall_collision(const Enemy* enemy, Vector2 new_position, const struct Map* current_map)` - Check wall collision

### Item API

#### `item.h`

Item system for collectible objects (coins, powerups, etc.).

**Item Types:**
```c
typedef enum {
    ITEM_TYPE_COIN,
    ITEM_TYPE_POWERUP,
    ITEM_TYPE_HEALTH_PACK,
    ITEM_TYPE_KEY,
    ITEM_TYPE_COUNT
} ItemType;
```

**Creation/Destruction:**
- `Item* item_create(ItemType type, Vector2 position)` - Create item instance
- `void item_destroy(Item* item)` - Destroy item instance

**State Queries:**
- `ItemType item_get_type(const Item* item)` - Get item type
- `Vector2 item_get_position(const Item* item)` - Get position
- `bool item_is_collected(const Item* item)` - Check if collected

**State Modifications:**
- `void item_set_position(Item* item, Vector2 position)` - Set position
- `void item_set_collected(Item* item, bool collected)` - Set collected state
- `void item_collect(Item* item)` - Mark as collected (triggers on_collect callback)

**Collision Detection:**
- `bool item_check_collision_with_player(const Item* item, Vector2 player_position, float player_radius)` - Check player collision

**Item Behavior:**
- `void item_on_collect(Item* item)` - Called when item is collected (extensible)
- `float item_get_radius(ItemType type)` - Get radius for item type
- `Color item_get_color(ItemType type)` - Get color for item type

## Creating Your Own Game

### Step 1: Create Game Structure

```c
#include "gengine.h"
#include "raylib.h"

typedef struct {
    bool running;
    int score;
    // Add your game state here
} MyGame;

MyGame* my_game_create(void) {
    MyGame* game = malloc(sizeof(MyGame));
    memset(game, 0, sizeof(MyGame));
    return game;
}

void my_game_destroy(MyGame* game) {
    if (game) free(game);
}
```

### Step 2: Implement Callbacks

```c
static void my_game_init(void* game_data) {
    MyGame* game = (MyGame*)game_data;
    game->running = true;
    game->score = 0;
    // Initialize your game
}

static void my_game_update(void* game_data, float delta_time) {
    MyGame* game = (MyGame*)game_data;
    
    // Handle input
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->running = false;
    }
    
    // Update game logic
    // ...
}

static void my_game_render(void* game_data) {
    MyGame* game = (MyGame*)game_data;
    
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    // Draw your game
    DrawText("My Game", 10, 10, 20, BLACK);
    
    EndDrawing();
}

static void my_game_cleanup(void* game_data) {
    MyGame* game = (MyGame*)game_data;
    // Cleanup resources
}
```

### Step 3: Register and Run

```c
int main(void) {
    // Create engine
    EngineConfig config = {
        .screen_width = 800,
        .screen_height = 600,
        .window_title = "My Game",
        .target_fps = 60
    };
    
    GameEngine* engine = gengine_create(&config);
    if (!engine) return 1;
    
    // Create game
    MyGame* game = my_game_create();
    if (!game) {
        gengine_destroy(engine);
        return 1;
    }
    
    // Register callbacks
    GameCallbacks callbacks = {
        .init = my_game_init,
        .update = my_game_update,
        .render = my_game_render,
        .cleanup = my_game_cleanup,
        .handle_input = NULL
    };
    
    gengine_register_game(engine, &callbacks, game);
    
    // Run
    gengine_run(engine);
    
    // Cleanup
    my_game_destroy(game);
    gengine_destroy(engine);
    
    return 0;
}
```

## Demo Game: Coin Collector

The included demo game demonstrates all engine features:

### Gameplay

1. **Start Screen**: Press SPACE or ENTER to start, H to view high scores
2. **Playing**: 
   - Use WASD or arrow keys to move
   - Click mouse or press SPACE+Arrow keys to shoot projectiles
   - Collect all coins across 4 maps
   - Avoid red obstacles (they damage you)
   - Walk into green exits to change maps
3. **Victory**: When all coins are collected, enter your name for the high score table
4. **High Scores**: View your best times (lowest frame count = best score)

### Game States

- `GAME_STATE_START` - Start/menu screen
- `GAME_STATE_PLAYING` - Main gameplay
- `GAME_STATE_ENTER_NAME` - Name entry after victory
- `GAME_STATE_END` - End/victory screen
- `GAME_STATE_HIGH_SCORES` - High scores display

### Key Features Demonstrated

- Multi-map navigation with exits/entrances
- Entity systems (player, enemies, items, projectiles)
- Collision detection
- Health system with invincibility frames
- Projectile combat system
- Persistent high score storage
- State management
- Audio feedback
- UI rendering

## Constants

### Screen Constants
- `SCREEN_WIDTH = 800`
- `SCREEN_HEIGHT = 600`

### Player Constants
- `PLAYER_SPEED = 5.0f`
- `PLAYER_RADIUS = 25.0f`
- `MAX_HEALTH = 100.0f`
- `DAMAGE_PER_HIT = 10.0f`
- `INVINCIBILITY_FRAMES = 60`

### Enemy Constants
- `ENEMY_SPEED = 2.0f`
- `ENEMY_RADIUS = 20.0f`
- `ENEMY_DIRECTION_CHANGE_FRAMES = 120`

### Projectile Constants
- `PROJECTILE_SPEED = 10.0f`
- `PROJECTILE_RADIUS = 5.0f`
- `PROJECTILE_DAMAGE = 20.0f`
- `PROJECTILE_LIFETIME = 120` (frames)
- `PROJECTILE_COOLDOWN = 10` (frames between shots)
- `MAX_PROJECTILES = 50`

### Item Constants
- `COIN_RADIUS = 15.0f`
- `ITEM_COIN_RADIUS = 15.0f`

### Map Constants
- `MAX_WALLS = 20`
- `MAX_EXITS = 4`
- `MAX_ENTRANCES = 4`
- `MAX_COINS = 10`
- `MAX_OBSTACLES = 5`
- `NUM_MAPS = 4`

### State Constants
- `MAX_HIGH_SCORES = 10`
- `MAX_NAME_LENGTH = 20`

## Input Handling

The engine uses Raylib's input system. Common functions:

- `IsKeyDown(KEY_W)` - Check if key is held
- `IsKeyPressed(KEY_SPACE)` - Check if key was just pressed
- `IsMouseButtonPressed(MOUSE_BUTTON_LEFT)` - Check if mouse button was pressed
- `GetMousePosition()` - Get current mouse position
- `GetCharPressed()` - Get character input
- `WindowShouldClose()` - Check if window close requested

## Best Practices

1. **Memory Management**: Always pair `create` functions with `destroy` functions
2. **Null Checks**: Always check for NULL before using pointers
3. **State Management**: Use the state module for centralized game state
4. **Modularity**: Keep game logic separate from rendering
5. **Constants**: Use constants instead of magic numbers
6. **Error Handling**: Check return values from functions

## Dependencies

- **Raylib**: Graphics, input, and audio library (automatically fetched via CMake)
- **CMake**: Build system
- **C11 Standard**: Modern C features

## Project Structure

```
.
├── CMakeLists.txt          # Build configuration
├── README.md              # This file
├── include/               # Header files
│   ├── audio.h
│   ├── enemy.h
│   ├── game.h
│   ├── gengine.h
│   ├── highscore.h
│   ├── item.h
│   ├── map.h
│   ├── player.h
│   ├── projectile.h
│   ├── renderer.h
│   └── state.h
├── src/                   # Source files
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
│   ├── renderer.c
│   └── state.c
└── build/                 # Build directory (generated)
```

## License

This project is provided as-is for educational and development purposes.

## Contributing

Contributions are welcome! Please ensure code follows the existing style and includes proper documentation.

## Troubleshooting

### Build Issues

- **CMake version**: Ensure CMake 3.16+ is installed
- **Raylib fetch fails**: Check internet connection, CMake will fetch raylib automatically
- **Compiler errors**: Ensure C11 standard is supported

### Runtime Issues

- **Audio not working**: Check system audio drivers
- **Window not appearing**: Check screen resolution settings
- **High scores not saving**: Check file write permissions in the executable directory

## Examples

See `src/game.c` for a complete example of implementing a game using the engine API.
