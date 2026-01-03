#include "../include/gengine.h"
#include "../include/audio.h"
#include <stdlib.h>
#include <stdio.h>

struct GameEngine {
    EngineConfig config;
    GameCallbacks callbacks;
    void* game_data;
    bool running;
    int frame_count;
    bool initialized;
};

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
    
    engine->callbacks.init = NULL;
    engine->callbacks.update = NULL;
    engine->callbacks.render = NULL;
    engine->callbacks.cleanup = NULL;
    engine->callbacks.handle_input = NULL;
    
    return engine;
}

void gengine_destroy(GameEngine* engine) {
    if (!engine) return;
    
    // Note: cleanup is already called in gengine_run(), so we don't call it here
    // to avoid double-cleanup and use-after-free issues
    
    if (engine->initialized) {
        audio_cleanup();
        engine->initialized = false;
    }
    
    if (IsWindowReady()) {
        CloseWindow();
    }
    
    free(engine);
}

void gengine_register_game(GameEngine* engine, GameCallbacks* callbacks, void* game_data) {
    if (!engine || !callbacks) return;
    
    engine->callbacks = *callbacks;
    engine->game_data = game_data;
}

void gengine_run(GameEngine* engine) {
    if (!engine) return;
    
    InitWindow(engine->config.screen_width, engine->config.screen_height, engine->config.window_title);
    SetTargetFPS(engine->config.target_fps);
    
    engine->initialized = audio_init();
    
    if (engine->callbacks.init && engine->game_data) {
        engine->callbacks.init(engine->game_data);
    }
    
    engine->running = true;
    
    while (engine->running && !WindowShouldClose()) {
        float delta_time = GetFrameTime();
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            engine->running = false;
        }
        
        if (engine->callbacks.update && engine->game_data) {
            engine->callbacks.update(engine->game_data, delta_time);
        }
        
        if (engine->callbacks.render && engine->game_data) {
            BeginDrawing();
            engine->callbacks.render(engine->game_data);
            EndDrawing();
        }
        
        engine->frame_count++;
    }
    
    if (engine->callbacks.cleanup && engine->game_data) {
        engine->callbacks.cleanup(engine->game_data);
    }
    
    if (engine->initialized) {
        audio_cleanup();
        engine->initialized = false;
    }
    CloseWindow();
}

int gengine_get_frame_count(GameEngine* engine) {
    if (!engine) return 0;
    return engine->frame_count;
}

bool gengine_is_running(GameEngine* engine) {
    if (!engine) return false;
    return engine->running;
}

void gengine_stop(GameEngine* engine) {
    if (!engine) return;
    engine->running = false;
}
