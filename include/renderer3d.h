#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "raylib.h"
#include "map.h"
#include "player.h"
#include "raycaster.h"
#include <stdbool.h>

/**
 * Initialize the 3D renderer.
 */
void renderer3d_init(void);

/**
 * Render a 3D view using ray casting.
 * @param map The current map
 * @param player_pos Player position
 * @param player_angle Player viewing angle in radians (0 = right, PI/2 = down)
 * @param health Current health
 * @param max_health Maximum health
 * @param current_map_id Current map ID
 * @param coins_collected Number of coins collected
 * @param enemy_positions Array of enemy positions
 * @param enemy_count Number of enemies
 * @param enemy_colors Array of enemy colors
 * @param coin_positions Array of coin positions
 * @param coin_count Number of coins
 * @param coin_collected Array indicating which coins are collected
 */
void renderer3d_render(const Map* map, Vector2 player_pos, float player_angle,
                       float health, float max_health, int current_map_id, int coins_collected,
                       Vector2* enemy_positions, int enemy_count, Color* enemy_colors,
                       Vector2* coin_positions, int coin_count, bool* coin_collected);

/**
 * Draw a minimap overlay.
 * @param map The current map
 * @param player_pos Player position
 * @param player_angle Player viewing angle
 * @param x X position of minimap
 * @param y Y position of minimap
 * @param size Size of minimap
 */
void renderer3d_draw_minimap(const Map* map, Vector2 player_pos, float player_angle,
                            int x, int y, int size);

#endif

