#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "map.h"
#include "highscore.h"
#include "projectile.h"
#include <stdbool.h>
#include <stddef.h>

#define COIN_RADIUS 15.0f
#define PLAYER_RADIUS 25.0f

/**
 * Initialize the renderer system.
 */
void renderer_init(void);

/**
 * Clear the screen with a specific color.
 * @param color The color to clear with
 */
void renderer_clear(Color color);

/**
 * Draw a map including walls and exits.
 * @param map The map to draw
 */
void renderer_draw_map(const Map* map);

/**
 * Draw a coin at the specified position.
 * @param position Position to draw the coin
 * @param collected Whether the coin has been collected
 */
void renderer_draw_coin(Vector2 position, bool collected);

/**
 * Draw an obstacle at the specified position.
 * @param position Position to draw the obstacle
 * @param radius Radius of the obstacle
 * @param color Color of the obstacle
 */
void renderer_draw_obstacle(Vector2 position, float radius, Color color);

/**
 * Draw the player at the specified position.
 * @param position Position to draw the player
 * @param invincible Whether the player is currently invincible
 * @param invincibility_timer Current invincibility timer value
 */
void renderer_draw_player(Vector2 position, bool invincible, int invincibility_timer);

/**
 * Draw a health bar.
 * @param x X position of the health bar
 * @param y Y position of the health bar
 * @param width Width of the health bar
 * @param height Height of the health bar
 * @param health Current health value
 * @param max_health Maximum health value
 */
void renderer_draw_health_bar(float x, float y, float width, float height, float health, float max_health);

/**
 * Draw text centered horizontally on the screen.
 * @param text Text to draw
 * @param y Y position of the text
 * @param font_size Font size
 * @param color Text color
 */
void renderer_draw_text_centered(const char* text, int y, int font_size, Color color);

/**
 * Draw text at a specific position.
 * @param text Text to draw
 * @param x X position of the text
 * @param y Y position of the text
 * @param font_size Font size
 * @param color Text color
 */
void renderer_draw_text(const char* text, int x, int y, int font_size, Color color);

/**
 * Draw FPS counter.
 * @param x X position
 * @param y Y position
 */
void renderer_draw_fps(int x, int y);

/**
 * Draw the start screen.
 * @param frame_count Current frame count
 * @param high_scores Array of high scores
 * @param high_score_count Number of high scores
 */
void renderer_draw_start_screen(int frame_count, const HighScore* high_scores, int high_score_count);

/**
 * Draw the end/victory screen.
 * @param frame_count Current frame count
 * @param game_start_frame Frame count when game started
 * @param total_coins Total number of coins in the game
 * @param health Current health
 * @param max_health Maximum health
 * @param high_scores Array of high scores
 * @param high_score_count Number of high scores
 */
void renderer_draw_end_screen(int frame_count, int game_start_frame, int total_coins, float health, float max_health, 
                              const HighScore* high_scores, int high_score_count);

/**
 * Draw the name entry screen.
 * @param player_name Current player name being entered
 * @param name_length Length of the current name
 * @param frame_count Current frame count
 * @param coins_collected Number of coins collected
 * @param health_remaining Remaining health
 */
void renderer_draw_name_entry_screen(const char* player_name, int name_length, int frame_count, int coins_collected, float health_remaining);

/**
 * Draw the high scores screen.
 * @param high_scores Array of high scores
 * @param high_score_count Number of high scores
 */
void renderer_draw_high_scores_screen(const HighScore* high_scores, int high_score_count);

/**
 * Draw a projectile.
 * @param position Position to draw the projectile
 * @param radius Radius of the projectile
 */
void renderer_draw_projectile(Vector2 position, float radius);

/**
 * Draw the main game screen.
 * @param current_map Current map being played
 * @param player_position Player's position
 * @param invincible Whether player is invincible
 * @param invincibility_timer Current invincibility timer
 * @param health Current health
 * @param max_health Maximum health
 * @param current_map_id Current map ID
 * @param coins_collected Number of coins collected
 * @param projectiles Array of projectiles to draw
 * @param projectile_count Number of projectiles
 */
void renderer_draw_game_screen(const Map* current_map, Vector2 player_position, bool invincible, int invincibility_timer,
                              float health, float max_health, int current_map_id, int coins_collected,
                              Projectile** projectiles, int projectile_count);

#endif
