#ifndef GENGINE_H
#define GENGINE_H

#include "raylib.h"
#include <stdbool.h>

typedef struct GameEngine GameEngine;
typedef struct GameInterface GameInterface;

typedef struct {
    void (*init)(void* game_data);
    void (*update)(void* game_data, float delta_time);
    void (*render)(void* game_data);
    void (*cleanup)(void* game_data);
    void (*handle_input)(void* game_data, int key);
} GameCallbacks;

typedef struct {
    int screen_width;
    int screen_height;
    const char* window_title;
    int target_fps;
} EngineConfig;

/**
 * Create a new game engine instance.
 * @param config Engine configuration
 * @return Pointer to created engine, or NULL on failure
 */
GameEngine* gengine_create(const EngineConfig* config);

/**
 * Destroy a game engine instance.
 * @param engine The engine to destroy
 */
void gengine_destroy(GameEngine* engine);

/**
 * Register game callbacks with the engine.
 * @param engine The engine
 * @param callbacks Game callbacks structure
 * @param game_data Game-specific data pointer
 */
void gengine_register_game(GameEngine* engine, GameCallbacks* callbacks, void* game_data);

/**
 * Run the engine main loop.
 * @param engine The engine to run
 */
void gengine_run(GameEngine* engine);

/**
 * Get the current frame count.
 * @param engine The engine
 * @return Current frame count
 */
int gengine_get_frame_count(GameEngine* engine);

/**
 * Check if the engine is currently running.
 * @param engine The engine
 * @return true if running, false otherwise
 */
bool gengine_is_running(GameEngine* engine);

/**
 * Request the engine to stop.
 * @param engine The engine
 */
void gengine_stop(GameEngine* engine);

#endif
