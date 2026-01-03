#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "map.h"
#include <stdbool.h>
#include <stddef.h>

// Forward declarations (avoid circular dependencies)
struct HighScore;

// Renderer service interface (Dependency Inversion Principle)
// Games depend on this abstraction, not concrete rendering implementation

// Rendering constants
#define COIN_RADIUS 15.0f
#define PLAYER_RADIUS 25.0f

// Initialize renderer
void renderer_init(void);

// Clear screen with color
void renderer_clear(Color color);

// Map rendering (Single Responsibility - map-specific rendering)
void renderer_draw_map(const Map* map);

// Entity rendering (Interface Segregation - separate entity types)
void renderer_draw_coin(Vector2 position, bool collected);
void renderer_draw_obstacle(Vector2 position, float radius, Color color);
void renderer_draw_player(Vector2 position, bool invincible, int invincibility_timer);

// UI rendering (Interface Segregation - separate UI components)
void renderer_draw_health_bar(float x, float y, float width, float height, float health, float max_health);
void renderer_draw_text_centered(const char* text, int y, int font_size, Color color);
void renderer_draw_text(const char* text, int x, int y, int font_size, Color color);
void renderer_draw_fps(int x, int y);

// Screen rendering (Interface Segregation - separate screen types)
// Start screen
void renderer_draw_start_screen(int frame_count, const struct HighScore* high_scores, int high_score_count);

// End screen  
void renderer_draw_end_screen(int frame_count, int game_start_frame, int total_coins, float health, float max_health, 
                              const struct HighScore* high_scores, int high_score_count);

// Name entry screen
void renderer_draw_name_entry_screen(const char* player_name, int name_length, int frame_count, int coins_collected, float health_remaining);

// High scores screen
void renderer_draw_high_scores_screen(const struct HighScore* high_scores, int high_score_count);

// Game screen
void renderer_draw_game_screen(const Map* current_map, Vector2 player_position, bool invincible, int invincibility_timer,
                              float health, float max_health, int current_map_id, int coins_collected);

#endif // RENDERER_H

