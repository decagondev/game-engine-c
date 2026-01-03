#ifndef GAME_H
#define GAME_H

#include "gengine.h"
#include "raylib.h"
#include <stdbool.h>

typedef struct CoinCollectorGame CoinCollectorGame;

/**
 * Create a new game instance.
 * @return Pointer to created game, or NULL on failure
 */
CoinCollectorGame* game_create(void);

/**
 * Destroy a game instance.
 * @param game The game to destroy
 */
void game_destroy(CoinCollectorGame* game);

/**
 * Get game callbacks for engine registration.
 * @param game The game
 * @return Game callbacks structure
 */
GameCallbacks game_get_callbacks(CoinCollectorGame* game);

/**
 * Get game data pointer.
 * @param game The game
 * @return Game data pointer
 */
void* game_get_data(CoinCollectorGame* game);

/**
 * Set engine reference in the game.
 * @param game The game
 * @param engine The engine
 */
void game_set_engine(CoinCollectorGame* game, GameEngine* engine);

#endif
