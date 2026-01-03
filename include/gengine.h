#ifndef GENGINE_H
#define GENGINE_H

#include "raylib.h"
#include <stdbool.h>

// Forward declarations
typedef struct GameEngine GameEngine;
typedef struct GameInterface GameInterface;

// Game state callback functions (Interface Segregation Principle)
// Each callback has a single responsibility
typedef struct {
    // Initialize game-specific resources
    void (*init)(void* game_data);
    
    // Update game logic
    void (*update)(void* game_data, float delta_time);
    
    // Render game
    void (*render)(void* game_data);
    
    // Cleanup game-specific resources
    void (*cleanup)(void* game_data);
    
    // Handle input
    void (*handle_input)(void* game_data, int key);
} GameCallbacks;

// Engine configuration
typedef struct {
    int screen_width;
    int screen_height;
    const char* window_title;
    int target_fps;
} EngineConfig;

// Engine interface (Dependency Inversion Principle)
GameEngine* gengine_create(const EngineConfig* config);
void gengine_destroy(GameEngine* engine);

// Register game callbacks (Open/Closed Principle - open for extension)
void gengine_register_game(GameEngine* engine, GameCallbacks* callbacks, void* game_data);

// Run the engine (main loop)
void gengine_run(GameEngine* engine);

// Get current frame count
int gengine_get_frame_count(GameEngine* engine);

// Check if engine is running
bool gengine_is_running(GameEngine* engine);

// Request engine to stop
void gengine_stop(GameEngine* engine);

#endif // GENGINE_H

