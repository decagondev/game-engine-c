#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

struct Map;

#define PLAYER_SPEED 5.0f
#define PLAYER_RADIUS 25.0f
#define MAX_HEALTH 100.0f
#define DAMAGE_PER_HIT 10.0f
#define INVINCIBILITY_FRAMES 60

typedef struct Player Player;

/**
 * Create a new player instance.
 * @return Pointer to created player, or NULL on failure
 */
Player* player_create(void);

/**
 * Destroy a player instance.
 * @param player The player to destroy
 */
void player_destroy(Player* player);

/**
 * Initialize a player with starting position.
 * @param player The player to initialize
 * @param start_position Starting position
 */
void player_init(Player* player, Vector2 start_position);

/**
 * Get the player's current position.
 * @param player The player
 * @return Current position
 */
Vector2 player_get_position(const Player* player);

/**
 * Get the player's current health.
 * @param player The player
 * @return Current health
 */
float player_get_health(const Player* player);

/**
 * Get the player's maximum health.
 * @param player The player
 * @return Maximum health
 */
float player_get_max_health(const Player* player);

/**
 * Check if the player is currently invincible.
 * @param player The player
 * @return true if invincible, false otherwise
 */
bool player_is_invincible(const Player* player);

/**
 * Get the current invincibility timer value.
 * @param player The player
 * @return Timer value
 */
int player_get_invincibility_timer(const Player* player);

/**
 * Check if the player is alive.
 * @param player The player
 * @return true if alive, false otherwise
 */
bool player_is_alive(const Player* player);

/**
 * Set the player's position.
 * @param player The player
 * @param position New position
 */
void player_set_position(Player* player, Vector2 position);

/**
 * Set the player's health.
 * @param player The player
 * @param health New health value
 */
void player_set_health(Player* player, float health);

/**
 * Reset the player to initial state.
 * @param player The player
 * @param start_position Starting position
 */
void player_reset(Player* player, Vector2 start_position);

/**
 * Update player movement based on input.
 * @param player The player
 * @param current_map Current map for collision detection
 */
void player_update_movement(Player* player, const struct Map* current_map);

/**
 * Handle player input.
 * @param player The player
 */
void player_handle_input(Player* player);

/**
 * Check if player would collide with a wall at new position.
 * @param player The player
 * @param new_position Position to check
 * @param current_map Current map
 * @return true if collision detected, false otherwise
 */
bool player_check_wall_collision(const Player* player, Vector2 new_position, const struct Map* current_map);

/**
 * Check if player is colliding with an exit.
 * @param player The player
 * @param current_map Current map
 * @param target_map_id Output parameter for target map ID
 * @param target_entrance_id Output parameter for target entrance ID
 * @return true if collision detected, false otherwise
 */
bool player_check_exit_collision(const Player* player, const struct Map* current_map, int* target_map_id, int* target_entrance_id);

/**
 * Apply damage to the player.
 * @param player The player
 * @param damage Amount of damage to apply
 */
void player_apply_damage(Player* player, float damage);

/**
 * Update player state (invincibility timer, etc.).
 * @param player The player
 */
void player_update(Player* player);

#endif
