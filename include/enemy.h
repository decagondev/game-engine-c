#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <stdbool.h>

struct Map;

#define ENEMY_SPEED 2.0f
#define ENEMY_RADIUS 20.0f
#define ENEMY_DIRECTION_CHANGE_FRAMES 120

typedef struct Enemy Enemy;

/**
 * Create a new enemy instance.
 * @param position Starting position
 * @param velocity Initial velocity
 * @param color Enemy color
 * @return Pointer to created enemy, or NULL on failure
 */
Enemy* enemy_create(Vector2 position, Vector2 velocity, Color color);

/**
 * Destroy an enemy instance.
 * @param enemy The enemy to destroy
 */
void enemy_destroy(Enemy* enemy);

/**
 * Get the enemy's current position.
 * @param enemy The enemy
 * @return Current position
 */
Vector2 enemy_get_position(const Enemy* enemy);

/**
 * Get the enemy's current velocity.
 * @param enemy The enemy
 * @return Current velocity
 */
Vector2 enemy_get_velocity(const Enemy* enemy);

/**
 * Get the enemy's radius.
 * @param enemy The enemy
 * @return Radius
 */
float enemy_get_radius(const Enemy* enemy);

/**
 * Get the enemy's color.
 * @param enemy The enemy
 * @return Color
 */
Color enemy_get_color(const Enemy* enemy);

/**
 * Get the enemy's direction change timer.
 * @param enemy The enemy
 * @return Timer value
 */
int enemy_get_direction_timer(const Enemy* enemy);

/**
 * Set the enemy's position.
 * @param enemy The enemy
 * @param position New position
 */
void enemy_set_position(Enemy* enemy, Vector2 position);

/**
 * Set the enemy's velocity.
 * @param enemy The enemy
 * @param velocity New velocity
 */
void enemy_set_velocity(Enemy* enemy, Vector2 velocity);

/**
 * Set the enemy's direction change timer.
 * @param enemy The enemy
 * @param timer New timer value
 */
void enemy_set_direction_timer(Enemy* enemy, int timer);

/**
 * Update enemy state and movement.
 * @param enemy The enemy
 * @param current_map Current map for collision detection
 */
void enemy_update(Enemy* enemy, const struct Map* current_map);

/**
 * Check if enemy is colliding with the player.
 * @param enemy The enemy
 * @param player_position Player's position
 * @param player_radius Player's radius
 * @return true if collision detected, false otherwise
 */
bool enemy_check_collision_with_player(const Enemy* enemy, Vector2 player_position, float player_radius);

/**
 * Check if enemy would collide with a wall at new position.
 * @param enemy The enemy
 * @param new_position Position to check
 * @param current_map Current map
 * @return true if collision detected, false otherwise
 */
bool enemy_check_wall_collision(const Enemy* enemy, Vector2 new_position, const struct Map* current_map);

#endif
