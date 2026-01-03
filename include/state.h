#ifndef STATE_H
#define STATE_H

#include "raylib.h"
#include "map.h"
#include "player.h"
#include "highscore.h"
#include "projectile.h"
#include <stdbool.h>

#define NUM_MAPS 4
#define MAX_NAME_LENGTH 20
#define MAX_PROJECTILES 50

typedef enum {
    GAME_STATE_START,
    GAME_STATE_PLAYING,
    GAME_STATE_END,
    GAME_STATE_ENTER_NAME,
    GAME_STATE_HIGH_SCORES
} GameStateType;

typedef struct GameState GameState;

/**
 * Create a new game state instance.
 * @return Pointer to created state, or NULL on failure
 */
GameState* state_create(void);

/**
 * Destroy a game state instance.
 * @param state The state to destroy
 */
void state_destroy(GameState* state);

/**
 * Initialize a game state.
 * @param state The state to initialize
 */
void state_init(GameState* state);

/**
 * Reset the game state to initial playing state.
 * @param state The state to reset
 */
void state_reset(GameState* state);

/**
 * Get the current state type.
 * @param state The state
 * @return Current state type
 */
GameStateType state_get_type(const GameState* state);

/**
 * Set the current state type.
 * @param state The state
 * @param type New state type
 */
void state_set_type(GameState* state, GameStateType type);

/**
 * Get the running flag.
 * @param state The state
 * @return true if running, false otherwise
 */
bool state_is_running(const GameState* state);

/**
 * Set the running flag.
 * @param state The state
 * @param running Running flag value
 */
void state_set_running(GameState* state, bool running);

/**
 * Get the frame count.
 * @param state The state
 * @return Frame count
 */
int state_get_frame_count(const GameState* state);

/**
 * Set the frame count.
 * @param state The state
 * @param frame_count Frame count value
 */
void state_set_frame_count(GameState* state, int frame_count);

/**
 * Increment the frame count.
 * @param state The state
 */
void state_increment_frame_count(GameState* state);

/**
 * Get the game start frame.
 * @param state The state
 * @return Game start frame
 */
int state_get_game_start_frame(const GameState* state);

/**
 * Set the game start frame.
 * @param state The state
 * @param frame Game start frame value
 */
void state_set_game_start_frame(GameState* state, int frame);

/**
 * Get the player.
 * @param state The state
 * @return Pointer to player, or NULL
 */
Player* state_get_player(GameState* state);

/**
 * Get the current map ID.
 * @param state The state
 * @return Current map ID
 */
int state_get_current_map_id(const GameState* state);

/**
 * Set the current map ID.
 * @param state The state
 * @param map_id Map ID value
 */
void state_set_current_map_id(GameState* state, int map_id);

/**
 * Get the coins collected count.
 * @param state The state
 * @return Coins collected count
 */
int state_get_coins_collected(const GameState* state);

/**
 * Set the coins collected count.
 * @param state The state
 * @param count Coins collected count
 */
void state_set_coins_collected(GameState* state, int count);

/**
 * Increment the coins collected count.
 * @param state The state
 */
void state_increment_coins_collected(GameState* state);

/**
 * Get the total coins count.
 * @param state The state
 * @return Total coins count
 */
int state_get_total_coins(const GameState* state);

/**
 * Get the maps array.
 * @param state The state
 * @return Pointer to maps array
 */
Map* state_get_maps(GameState* state);

/**
 * Get the current map.
 * @param state The state
 * @return Pointer to current map, or NULL
 */
Map* state_get_current_map(GameState* state);

/**
 * Get the high scores (const).
 * @param state The state
 * @param count Output parameter for high score count
 * @return Pointer to high scores array
 */
const HighScore* state_get_high_scores(const GameState* state, int* count);

/**
 * Get the high scores (mutable).
 * @param state The state
 * @param count Output parameter for high score count pointer
 * @return Pointer to high scores array
 */
HighScore* state_get_high_scores_mutable(GameState* state, int** count);

/**
 * Get the player name buffer.
 * @param state The state
 * @return Pointer to player name buffer
 */
char* state_get_player_name(GameState* state);

/**
 * Get the player name character count.
 * @param state The state
 * @return Name character count
 */
int state_get_name_char_count(const GameState* state);

/**
 * Set the player name character count.
 * @param state The state
 * @param count Character count value
 */
void state_set_name_char_count(GameState* state, int count);

/**
 * Increment the player name character count.
 * @param state The state
 */
void state_increment_name_char_count(GameState* state);

/**
 * Decrement the player name character count.
 * @param state The state
 */
void state_decrement_name_char_count(GameState* state);

/**
 * Get the pending score.
 * @param state The state
 * @return Pointer to pending score
 */
HighScore* state_get_pending_score(GameState* state);

/**
 * Check if all coins have been collected.
 * @param state The state
 * @return true if all coins collected, false otherwise
 */
bool state_all_coins_collected(const GameState* state);

/**
 * Reset all coins to uncollected state.
 * @param state The state
 */
void state_reset_coins(GameState* state);

/**
 * Add a projectile to the state.
 * @param state The state
 * @param projectile The projectile to add
 * @return true if added successfully, false if array is full
 */
bool state_add_projectile(GameState* state, Projectile* projectile);

/**
 * Get all projectiles.
 * @param state The state
 * @param count Output parameter for projectile count
 * @return Pointer to projectiles array
 */
Projectile** state_get_projectiles(GameState* state, int* count);

/**
 * Remove a projectile at the given index.
 * @param state The state
 * @param index Index of projectile to remove
 */
void state_remove_projectile(GameState* state, int index);

/**
 * Clear all projectiles.
 * @param state The state
 */
void state_clear_projectiles(GameState* state);

/**
 * Get the projectile cooldown.
 * @param state The state
 * @return Current cooldown value
 */
int state_get_projectile_cooldown(const GameState* state);

/**
 * Set the projectile cooldown.
 * @param state The state
 * @param cooldown Cooldown value
 */
void state_set_projectile_cooldown(GameState* state, int cooldown);

/**
 * Decrement the projectile cooldown.
 * @param state The state
 */
void state_decrement_projectile_cooldown(GameState* state);

#endif

