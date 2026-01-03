#include "../include/gengine.h"
#include <stdlib.h>
#include <stdio.h>

// Engine structure (Single Responsibility Principle)
struct GameEngine {
    EngineConfig config;
    GameCallbacks callbacks;
    void* game_data;
    bool running;
    int frame_count;
    bool initialized;
};

// Create engine instance
GameEngine* gengine_create(const EngineConfig* config) {
    GameEngine* engine = (GameEngine*)malloc(sizeof(GameEngine));
    if (!engine) {
        return NULL;
    }
    
    engine->config = *config;
    engine->running = false;
    engine->frame_count = 0;
    engine->initialized = false;
    engine->game_data = NULL;
    
    // Initialize callbacks to NULL
    engine->callbacks.init = NULL;
    engine->callbacks.update = NULL;
    engine->callbacks.render = NULL;
    engine->callbacks.cleanup = NULL;
    engine->callbacks.handle_input = NULL;
    
    return engine;
}

// Destroy engine instance
void gengine_destroy(GameEngine* engine) {
    if (!engine) return;
    
    // Cleanup game if registered
    if (engine->callbacks.cleanup && engine->game_data) {
        engine->callbacks.cleanup(engine->game_data);
    }
    
    // Close audio device if initialized
    if (engine->initialized) {
        CloseAudioDevice();
    }
    
    // Close window if initialized
    if (IsWindowReady()) {
        CloseWindow();
    }
    
    free(engine);
}

// Register game callbacks
void gengine_register_game(GameEngine* engine, GameCallbacks* callbacks, void* game_data) {
    if (!engine || !callbacks) return;
    
    engine->callbacks = *callbacks;
    engine->game_data = game_data;
}

// Run the engine
void gengine_run(GameEngine* engine) {
    if (!engine) return;
    
    // Initialize window
    InitWindow(engine->config.screen_width, engine->config.screen_height, engine->config.window_title);
    SetTargetFPS(engine->config.target_fps);
    
    // Initialize audio
    InitAudioDevice();
    engine->initialized = true;
    
    // Initialize game if callback provided
    if (engine->callbacks.init && engine->game_data) {
        engine->callbacks.init(engine->game_data);
    }
    
    engine->running = true;
    
    // Main game loop
    while (engine->running && !WindowShouldClose()) {
        // Calculate delta time
        float delta_time = GetFrameTime();
        
        // Handle input
        if (IsKeyPressed(KEY_ESCAPE)) {
            engine->running = false;
        }
        
        // Update game
        if (engine->callbacks.update && engine->game_data) {
            engine->callbacks.update(engine->game_data, delta_time);
        }
        
        // Render game
        if (engine->callbacks.render && engine->game_data) {
            BeginDrawing();
            engine->callbacks.render(engine->game_data);
            EndDrawing();
        }
        
        engine->frame_count++;
    }
    
    // Cleanup game
    if (engine->callbacks.cleanup && engine->game_data) {
        engine->callbacks.cleanup(engine->game_data);
    }
    
    // Cleanup engine resources
    CloseAudioDevice();
    CloseWindow();
}

// Get current frame count
int gengine_get_frame_count(GameEngine* engine) {
    if (!engine) return 0;
    return engine->frame_count;
}

// Check if engine is running
bool gengine_is_running(GameEngine* engine) {
    if (!engine) return false;
    return engine->running;
}

// Request engine to stop
void gengine_stop(GameEngine* engine) {
    if (!engine) return;
    engine->running = false;
}

