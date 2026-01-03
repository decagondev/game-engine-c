#include "../include/gengine.h"
#include "../include/game.h"
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_TITLE "Coin Collector Game"
#define TARGET_FPS 60

int main(void) {
    EngineConfig config = {
        .screen_width = SCREEN_WIDTH,
        .screen_height = SCREEN_HEIGHT,
        .window_title = WINDOW_TITLE,
        .target_fps = TARGET_FPS
    };
    
    GameEngine* engine = gengine_create(&config);
    if (!engine) {
        fprintf(stderr, "Failed to create game engine\n");
        return 1;
    }
    
    CoinCollectorGame* game = game_create();
    if (!game) {
        fprintf(stderr, "Failed to create game\n");
        gengine_destroy(engine);
        return 1;
    }
    
    game_set_engine(game, engine);
    
    GameCallbacks callbacks = game_get_callbacks(game);
    
    gengine_register_game(engine, &callbacks, game_get_data(game));
    
    gengine_run(engine);
    
    game_destroy(game);
    gengine_destroy(engine);
    
    return 0;
}
