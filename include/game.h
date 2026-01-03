#ifndef GAME_H
#define GAME_H

#include "gengine.h"
#include "raylib.h"
#include <stdbool.h>

// Game instance structure
typedef struct CoinCollectorGame CoinCollectorGame;

// Create game instance
CoinCollectorGame* game_create(void);

// Destroy game instance
void game_destroy(CoinCollectorGame* game);

// Get game callbacks for engine registration
GameCallbacks game_get_callbacks(CoinCollectorGame* game);

// Get game data pointer
void* game_get_data(CoinCollectorGame* game);

// Set engine reference (helper function)
void game_set_engine(CoinCollectorGame* game, GameEngine* engine);

#endif // GAME_H

