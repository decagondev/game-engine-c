#include "../include/renderer3d.h"
#include "../include/map.h"
#include "../include/player.h"
#include "../include/raycaster.h"
#include "../include/renderer.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FOV_RADIANS (RAYCASTER_FOV * DEG2RAD)

void renderer3d_init(void) {
    // Initialize 3D renderer if needed
}

void renderer3d_render(const Map* map, Vector2 player_pos, float player_angle,
                      float health, float max_health, int current_map_id, int coins_collected,
                      Vector2* enemy_positions, int enemy_count, Color* enemy_colors,
                      Vector2* coin_positions, int coin_count, bool* coin_collected) {
    if (!map) return;
    
    // Clear screen with floor and ceiling colors (gradient effect)
    Color floor_color_dark = (Color){30, 30, 30, 255};
    Color floor_color_light = (Color){60, 60, 60, 255};
    Color ceiling_color_dark = (Color){80, 80, 100, 255};
    Color ceiling_color_light = (Color){120, 120, 140, 255};
    
    // Draw gradient ceiling
    for (int y = 0; y < SCREEN_HEIGHT / 2; y++) {
        float t = (float)y / (SCREEN_HEIGHT / 2.0f);
        Color c = {
            (unsigned char)(ceiling_color_dark.r + (ceiling_color_light.r - ceiling_color_dark.r) * t),
            (unsigned char)(ceiling_color_dark.g + (ceiling_color_light.g - ceiling_color_dark.g) * t),
            (unsigned char)(ceiling_color_dark.b + (ceiling_color_light.b - ceiling_color_dark.b) * t),
            255
        };
        DrawLine(0, y, SCREEN_WIDTH, y, c);
    }
    
    // Draw gradient floor
    for (int y = SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT; y++) {
        float t = (float)(y - SCREEN_HEIGHT / 2) / (SCREEN_HEIGHT / 2.0f);
        Color c = {
            (unsigned char)(floor_color_light.r + (floor_color_dark.r - floor_color_light.r) * t),
            (unsigned char)(floor_color_light.g + (floor_color_dark.g - floor_color_light.g) * t),
            (unsigned char)(floor_color_light.b + (floor_color_dark.b - floor_color_light.b) * t),
            255
        };
        DrawLine(0, y, SCREEN_WIDTH, y, c);
    }
    
    // Store wall rendering data for depth sorting
    typedef struct {
        int x;
        int top;
        int bottom;
        Color color;
        float distance;
    } WallStrip;
    
    static WallStrip wall_strips[SCREEN_WIDTH];
    int strip_count = 0;
    
    // Cast rays for each column of the screen
    int num_rays = SCREEN_WIDTH;
    float ray_angle_step = FOV_RADIANS / num_rays;
    float start_angle = player_angle - FOV_RADIANS / 2.0f;
    
    for (int x = 0; x < num_rays; x++) {
        float ray_angle = start_angle + x * ray_angle_step;
        
        // Cast ray
        RaycastResult result = raycaster_cast_ray(player_pos, ray_angle, map);
        
        if (result.hit) {
            // Use perpendicular distance from result
            float perp_distance = result.perp_distance;
            float wall_height = result.wall_height;
            
            // Calculate top and bottom of wall strip
            int wall_top = (SCREEN_HEIGHT / 2) - (int)(wall_height / 2.0f);
            int wall_bottom = (SCREEN_HEIGHT / 2) + (int)(wall_height / 2.0f);
            
            if (wall_top < 0) wall_top = 0;
            if (wall_bottom > SCREEN_HEIGHT) wall_bottom = SCREEN_HEIGHT;
            
            // Store wall strip for rendering
            if (strip_count < SCREEN_WIDTH) {
                wall_strips[strip_count].x = x;
                wall_strips[strip_count].top = wall_top;
                wall_strips[strip_count].bottom = wall_bottom;
                wall_strips[strip_count].color = result.color;
                wall_strips[strip_count].distance = perp_distance;
                strip_count++;
            }
        }
    }
    
    // Render walls
    for (int i = 0; i < strip_count; i++) {
        DrawLine(wall_strips[i].x, wall_strips[i].top, wall_strips[i].x, wall_strips[i].bottom, wall_strips[i].color);
    }
    
    // Render enemies as sprites
    if (enemy_positions && enemy_colors && enemy_count > 0) {
        float enemy_distances[50];
        float sprite_angles[50];
        int enemy_indices[50];  // Store original indices
        int visible_count = raycaster_find_visible_enemies(player_pos, player_angle, map, RAYCASTER_MAX_DISTANCE,
                                                           enemy_positions, enemy_count, enemy_distances, sprite_angles);
        
        // Initialize indices
        for (int i = 0; i < visible_count; i++) {
            enemy_indices[i] = i;
        }
        
        // Sort enemies by distance (back to front for proper rendering) using indices
        for (int i = 0; i < visible_count - 1; i++) {
            for (int j = i + 1; j < visible_count; j++) {
                if (enemy_distances[enemy_indices[i]] < enemy_distances[enemy_indices[j]]) {
                    // Swap indices
                    int temp_idx = enemy_indices[i];
                    enemy_indices[i] = enemy_indices[j];
                    enemy_indices[j] = temp_idx;
                }
            }
        }
        
        // Render enemies from back to front
        for (int i = 0; i < visible_count; i++) {
            int idx = enemy_indices[i];
            float dist = enemy_distances[idx];
            float sprite_angle = sprite_angles[idx];
            
            // Calculate sprite size based on distance
            float sprite_size = (50.0f / dist) * 200.0f;
            if (sprite_size < 5.0f) sprite_size = 5.0f;
            if (sprite_size > 100.0f) sprite_size = 100.0f;
            
            // Calculate screen position
            float screen_x = SCREEN_WIDTH / 2.0f + (sprite_angle / (FOV_RADIANS / 2.0f)) * (SCREEN_WIDTH / 2.0f);
            float screen_y = SCREEN_HEIGHT / 2.0f;
            
            // Apply distance shading
            float shade = 1.0f / (1.0f + dist * 0.005f);
            if (shade < 0.3f) shade = 0.3f;
            if (shade > 1.0f) shade = 1.0f;
            
            Color sprite_color = {
                (unsigned char)(enemy_colors[idx].r * shade),
                (unsigned char)(enemy_colors[idx].g * shade),
                (unsigned char)(enemy_colors[idx].b * shade),
                255
            };
            
            // Draw enemy as a circle (sprite)
            DrawCircle((int)screen_x, (int)screen_y, sprite_size / 2.0f, sprite_color);
            DrawCircleLines((int)screen_x, (int)screen_y, sprite_size / 2.0f, 
                           (Color){sprite_color.r / 2, sprite_color.g / 2, sprite_color.b / 2, 255});
        }
    }
    
    // Render coins as sprites
    if (coin_positions && coin_collected && coin_count > 0) {
        float coin_distances[50];
        float coin_sprite_angles[50];
        int coin_indices[50];  // Store original indices
        int visible_coin_count = 0;
        float fov_half = FOV_RADIANS / 2.0f;
        
        // Find visible coins
        for (int i = 0; i < coin_count && visible_coin_count < 50; i++) {
            // Skip collected coins
            if (coin_collected[i]) continue;
            
            Vector2 coin_pos = coin_positions[i];
            Vector2 to_coin = {
                coin_pos.x - player_pos.x,
                coin_pos.y - player_pos.y
            };
            
            float dist = sqrtf(to_coin.x * to_coin.x + to_coin.y * to_coin.y);
            
            if (dist > RAYCASTER_MAX_DISTANCE || dist < 0.1f) continue;
            
            // Check if coin is within FOV
            float coin_angle = atan2f(to_coin.y, to_coin.x);
            float angle_diff = coin_angle - player_angle;
            
            // Normalize angle difference to [-PI, PI]
            while (angle_diff > PI) angle_diff -= 2.0f * PI;
            while (angle_diff < -PI) angle_diff += 2.0f * PI;
            
            if (fabsf(angle_diff) <= fov_half) {
                // Check if there's a wall blocking the view
                RaycastResult result = raycaster_cast_ray(player_pos, coin_angle, map);
                if (!result.hit || result.distance > dist) {
                    coin_distances[visible_coin_count] = dist;
                    coin_sprite_angles[visible_coin_count] = angle_diff;
                    coin_indices[visible_coin_count] = i;
                    visible_coin_count++;
                }
            }
        }
        
        // Sort coins by distance (back to front for proper rendering) using indices
        for (int i = 0; i < visible_coin_count - 1; i++) {
            for (int j = i + 1; j < visible_coin_count; j++) {
                if (coin_distances[coin_indices[i]] < coin_distances[coin_indices[j]]) {
                    // Swap indices
                    int temp_idx = coin_indices[i];
                    coin_indices[i] = coin_indices[j];
                    coin_indices[j] = temp_idx;
                }
            }
        }
        
        // Render coins from back to front
        for (int i = 0; i < visible_coin_count; i++) {
            int idx = coin_indices[i];
            float dist = coin_distances[idx];
            float sprite_angle = coin_sprite_angles[idx];
            
            // Calculate sprite size based on distance (coins are smaller than enemies)
            float sprite_size = (30.0f / dist) * 200.0f;
            if (sprite_size < 3.0f) sprite_size = 3.0f;
            if (sprite_size > 60.0f) sprite_size = 60.0f;
            
            // Calculate screen position
            float screen_x = SCREEN_WIDTH / 2.0f + (sprite_angle / (FOV_RADIANS / 2.0f)) * (SCREEN_WIDTH / 2.0f);
            float screen_y = SCREEN_HEIGHT / 2.0f;
            
            // Apply distance shading
            float shade = 1.0f / (1.0f + dist * 0.005f);
            if (shade < 0.4f) shade = 0.4f;  // Coins stay brighter than enemies
            if (shade > 1.0f) shade = 1.0f;
            
            // Coin colors (gold/yellow)
            Color coin_color = {
                (unsigned char)(255 * shade),
                (unsigned char)(215 * shade),
                (unsigned char)(0 * shade),
                255
            };
            
            // Draw coin as a circle with gold color
            DrawCircle((int)screen_x, (int)screen_y, sprite_size / 2.0f, coin_color);
            DrawCircleLines((int)screen_x, (int)screen_y, sprite_size / 2.0f, 
                           (Color){(unsigned char)(coin_color.r * 0.7f), 
                                  (unsigned char)(coin_color.g * 0.7f), 
                                  (unsigned char)(coin_color.b * 0.7f), 255});
            
            // Add inner highlight for coin effect
            DrawCircle((int)screen_x, (int)screen_y, sprite_size / 3.0f, 
                      (Color){(unsigned char)(255 * shade), 
                             (unsigned char)(255 * shade), 
                             (unsigned char)(100 * shade), 255});
        }
    }
    
    // Draw UI overlay
    renderer_draw_health_bar(SCREEN_WIDTH - 220, 20, 200, 20, health, max_health);
    
    char map_text[50];
    snprintf(map_text, sizeof(map_text), "Map: %d", current_map_id);
    renderer_draw_text(map_text, 10, 10, 20, WHITE);
    
    char coin_text[50];
    snprintf(coin_text, sizeof(coin_text), "Coins: %d", coins_collected);
    renderer_draw_text(coin_text, 10, 35, 20, GOLD);
    
    renderer_draw_text("WASD to move, Mouse to look, Q/E to turn", 10, 60, 18, WHITE);
    renderer_draw_fps(10, 85);
    
    // Draw minimap
    renderer3d_draw_minimap(map, player_pos, player_angle, SCREEN_WIDTH - 150, SCREEN_HEIGHT - 150, 140);
}

void renderer3d_draw_minimap(const Map* map, Vector2 player_pos, float player_angle,
                            int x, int y, int size) {
    if (!map) return;
    
    // Draw minimap background
    DrawRectangle(x, y, size, size, (Color){20, 20, 20, 200});
    DrawRectangleLines(x, y, size, size, WHITE);
    
    // Scale factor for minimap
    float scale = size / 800.0f;  // Assuming map is 800x600
    
    // Draw walls
    int wall_count;
    const Wall* walls = map_get_walls(map, &wall_count);
    for (int i = 0; i < wall_count; i++) {
        Rectangle wall = walls[i].rect;
        Rectangle minimap_wall = {
            x + wall.x * scale,
            y + wall.y * scale,
            wall.width * scale,
            wall.height * scale
        };
        DrawRectangleRec(minimap_wall, GRAY);
        DrawRectangleLinesEx(minimap_wall, 1, DARKGRAY);
    }
    
    // Draw exits
    int exit_count;
    const Exit* exits = map_get_exits(map, &exit_count);
    for (int i = 0; i < exit_count; i++) {
        Rectangle exit = exits[i].rect;
        Rectangle minimap_exit = {
            x + exit.x * scale,
            y + exit.y * scale,
            exit.width * scale,
            exit.height * scale
        };
        DrawRectangleRec(minimap_exit, GREEN);
        DrawRectangleLinesEx(minimap_exit, 1, DARKGREEN);
    }
    
    // Draw player position and direction
    Vector2 minimap_player_pos = {
        x + player_pos.x * scale,
        y + player_pos.y * scale
    };
    
    // Draw player as a circle
    DrawCircleV(minimap_player_pos, 3, BLUE);
    
    // Draw direction line
    Vector2 direction_end = {
        minimap_player_pos.x + cosf(player_angle) * 10,
        minimap_player_pos.y + sinf(player_angle) * 10
    };
    DrawLineV(minimap_player_pos, direction_end, YELLOW);
}

