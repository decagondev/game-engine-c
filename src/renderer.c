#include "../include/renderer.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_HIGH_SCORES 10
#define MAX_NAME_LENGTH 20

struct HighScore {
    char name[MAX_NAME_LENGTH + 1];
    int frame_count;
    int coins_collected;
    float health_remaining;
};

void renderer_init(void) {
}

void renderer_clear(Color color) {
    ClearBackground(color);
}

void renderer_draw_map(const Map* map) {
    if (!map) return;
    
    ClearBackground(map_get_background_color(map));
    
    int wall_count;
    const Wall* walls = map_get_walls(map, &wall_count);
    for (int i = 0; i < wall_count; i++) {
        DrawRectangleRec(walls[i].rect, DARKGRAY);
        DrawRectangleLinesEx(walls[i].rect, 2, BLACK);
    }
    
    int exit_count;
    const Exit* exits = map_get_exits(map, &exit_count);
    for (int i = 0; i < exit_count; i++) {
        DrawRectangleRec(exits[i].rect, GREEN);
        DrawRectangleLinesEx(exits[i].rect, 3, DARKGREEN);
        Vector2 center = {exits[i].rect.x + exits[i].rect.width/2, exits[i].rect.y + exits[i].rect.height/2};
        DrawText("→", center.x - 10, center.y - 10, 20, WHITE);
    }
}

void renderer_draw_coin(Vector2 position, bool collected) {
    if (collected) return;
    DrawCircleV(position, COIN_RADIUS, GOLD);
    DrawCircleV(position, COIN_RADIUS - 2, YELLOW);
    DrawCircleLinesV(position, COIN_RADIUS, ORANGE);
}

void renderer_draw_obstacle(Vector2 position, float radius, Color color) {
    DrawCircleV(position, radius, color);
    DrawCircleV(position, radius - 2, MAROON);
    DrawCircleLinesV(position, radius, BLACK);
    float size = radius * 0.6f;
    DrawLineEx((Vector2){position.x - size, position.y - size}, (Vector2){position.x + size, position.y + size}, 3, WHITE);
    DrawLineEx((Vector2){position.x - size, position.y + size}, (Vector2){position.x + size, position.y - size}, 3, WHITE);
}

void renderer_draw_player(Vector2 position, bool invincible, int invincibility_timer) {
    Color player_color = BLUE;
    Color player_inner_color = DARKBLUE;
    if (invincible && invincibility_timer > 0) {
        if ((invincibility_timer / 5) % 2 == 0) {
            player_color = (Color){player_color.r, player_color.g, player_color.b, 128};
            player_inner_color = (Color){player_inner_color.r, player_inner_color.g, player_inner_color.b, 128};
        } else {
            player_color = (Color){player_color.r, player_color.g, player_color.b, 255};
            player_inner_color = (Color){player_inner_color.r, player_inner_color.g, player_inner_color.b, 255};
        }
    }
    DrawCircleV(position, PLAYER_RADIUS, player_color);
    DrawCircleV(position, PLAYER_RADIUS - 2, player_inner_color);
}

void renderer_draw_health_bar(float x, float y, float width, float height, float health, float max_health) {
    DrawRectangle(x, y, width, height, GRAY);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 2, BLACK);
    
    float health_percentage = health / max_health;
    if (health_percentage < 0) health_percentage = 0;
    float health_fill_width = width * health_percentage;
    
    Color health_color;
    if (health_percentage > 0.6f) {
        health_color = GREEN;
    } else if (health_percentage > 0.3f) {
        health_color = YELLOW;
    } else {
        health_color = RED;
    }
    
    DrawRectangle(x, y, health_fill_width, height, health_color);
    
    char health_text[50];
    snprintf(health_text, sizeof(health_text), "HP: %.0f/%.0f", health, max_health);
    DrawText(health_text, x, y + height + 5, 16, BLACK);
}

void renderer_draw_text_centered(const char* text, int y, int font_size, Color color) {
    int width = MeasureText(text, font_size);
    DrawText(text, SCREEN_WIDTH/2 - width/2, y, font_size, color);
}

void renderer_draw_text(const char* text, int x, int y, int font_size, Color color) {
    DrawText(text, x, y, font_size, color);
}

void renderer_draw_fps(int x, int y) {
    DrawFPS(x, y);
}

void renderer_draw_start_screen(int frame_count, const struct HighScore* high_scores, int high_score_count) {
    ClearBackground((Color){30, 30, 50, 255});
    
    renderer_draw_text_centered("COIN COLLECTOR", 150, 60, GOLD);
    renderer_draw_text_centered("Collect all coins across 4 maps!", 240, 24, WHITE);
    renderer_draw_text_centered("Use WASD to move", 280, 20, LIGHTGRAY);
    renderer_draw_text_centered("Walk into green exits to change maps", 310, 20, LIGHTGRAY);
    renderer_draw_text_centered("Avoid red obstacles - they will kill you!", 340, 20, RED);
    renderer_draw_text_centered("Press H to view high scores", 380, 18, YELLOW);
    
    if ((frame_count / 30) % 2 == 0) {
        renderer_draw_text_centered("Press SPACE or ENTER to start", 450, 28, YELLOW);
    }
    
    for (int i = 0; i < 5; i++) {
        float x = 150 + i * 125;
        float y = 500;
        DrawCircleV((Vector2){x, y}, COIN_RADIUS, GOLD);
        DrawCircleV((Vector2){x, y}, COIN_RADIUS - 2, YELLOW);
        DrawCircleLinesV((Vector2){x, y}, COIN_RADIUS, ORANGE);
    }
    
    if (high_score_count > 0) {
        renderer_draw_text_centered("TOP SCORES (Press H for full list)", 420, 18, GOLD);
        int y_offset = 445;
        int max_display = (high_score_count < 3) ? high_score_count : 3;
        for (int i = 0; i < max_display; i++) {
            char score_text[120];
            snprintf(score_text, sizeof(score_text), "%d. %s - %d frames", 
                     i + 1, high_scores[i].name, high_scores[i].frame_count);
            renderer_draw_text_centered(score_text, y_offset, 16, WHITE);
            y_offset += 18;
        }
    }
}

void renderer_draw_end_screen(int frame_count, int game_start_frame, int total_coins, float health, float max_health,
                              const struct HighScore* high_scores, int high_score_count) {
    ClearBackground((Color){20, 50, 20, 255});
    
    renderer_draw_text_centered("VICTORY!", 150, 70, GOLD);
    
    char stats[100];
    snprintf(stats, sizeof(stats), "You collected all %d coins!", total_coins);
    renderer_draw_text_centered(stats, 220, 32, WHITE);
    
    int completion_frames = frame_count - game_start_frame;
    char frames[100];
    snprintf(frames, sizeof(frames), "Completion time: %d frames", completion_frames);
    renderer_draw_text_centered(frames, 260, 24, LIGHTGRAY);
    
    char health_text[100];
    snprintf(health_text, sizeof(health_text), "Health remaining: %.0f/%.0f", health, max_health);
    renderer_draw_text_centered(health_text, 290, 24, LIGHTGRAY);
    
    if (high_score_count > 0) {
        renderer_draw_text_centered("HIGH SCORES (Lowest frames = Best)", 330, 20, GOLD);
        int y_offset = 355;
        int max_display = (high_score_count < 5) ? high_score_count : 5;
        for (int i = 0; i < max_display; i++) {
            char score_text[150];
            snprintf(score_text, sizeof(score_text), "%d. %s - %d frames | Coins: %d | HP: %.0f", 
                     i + 1, high_scores[i].name, high_scores[i].frame_count, 
                     high_scores[i].coins_collected, high_scores[i].health_remaining);
            renderer_draw_text_centered(score_text, y_offset, 16, WHITE);
            y_offset += 18;
        }
    }
    
    if ((frame_count / 30) % 2 == 0) {
        renderer_draw_text_centered("Press SPACE or ENTER to play again", 480, 28, YELLOW);
    }
    renderer_draw_text_centered("Press ESC to quit", 520, 24, LIGHTGRAY);
    
    for (int i = 0; i < 8; i++) {
        float angle = (frame_count * 2 + i * 45) * DEG2RAD;
        float radius = 100;
        float x = SCREEN_WIDTH/2 + cosf(angle) * radius;
        float y = SCREEN_HEIGHT/2 + 50 + sinf(angle) * radius;
        DrawCircleV((Vector2){x, y}, COIN_RADIUS, GOLD);
        DrawCircleV((Vector2){x, y}, COIN_RADIUS - 2, YELLOW);
        DrawCircleLinesV((Vector2){x, y}, COIN_RADIUS, ORANGE);
    }
}

void renderer_draw_name_entry_screen(const char* player_name, int name_length, int frame_count, int coins_collected, float health_remaining) {
    ClearBackground((Color){30, 30, 50, 255});
    
    renderer_draw_text_centered("ENTER YOUR NAME", 200, 50, GOLD);
    
    char score_info[150];
    snprintf(score_info, sizeof(score_info), "Frames: %d | Coins: %d | HP: %.0f", 
             frame_count, coins_collected, health_remaining);
    renderer_draw_text_centered(score_info, 280, 24, WHITE);
    
    int box_width = 400;
    int box_height = 50;
    int box_x = SCREEN_WIDTH/2 - box_width/2;
    int box_y = 350;
    
    DrawRectangle(box_x, box_y, box_width, box_height, DARKGRAY);
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_width, box_height}, 3, WHITE);
    
    char display_text[MAX_NAME_LENGTH + 2];
    snprintf(display_text, sizeof(display_text), "%s_", player_name);
    DrawText(display_text, box_x + 10, box_y + 10, 32, WHITE);
    
    renderer_draw_text_centered("Type your name and press ENTER", 420, 20, LIGHTGRAY);
}

void renderer_draw_high_scores_screen(const struct HighScore* high_scores, int high_score_count) {
    ClearBackground((Color){20, 20, 40, 255});
    
    renderer_draw_text_centered("HIGH SCORES", 50, 60, GOLD);
    renderer_draw_text_centered("Lowest frames = Best score", 120, 20, LIGHTGRAY);
    
    if (high_score_count > 0) {
        int y_offset = 180;
        int max_display = (high_score_count < MAX_HIGH_SCORES) ? high_score_count : MAX_HIGH_SCORES;
        
        renderer_draw_text_centered("Rank  Name                Frames  Coins  HP", y_offset, 18, YELLOW);
        y_offset += 35;
        
        for (int i = 0; i < max_display; i++) {
            char score_text[200];
            snprintf(score_text, sizeof(score_text), "%2d.   %-20s %6d  %5d  %.0f", 
                     i + 1, high_scores[i].name, high_scores[i].frame_count, 
                     high_scores[i].coins_collected, high_scores[i].health_remaining);
            renderer_draw_text_centered(score_text, y_offset, 18, WHITE);
            y_offset += 25;
        }
    } else {
        renderer_draw_text_centered("No high scores yet!", 300, 32, LIGHTGRAY);
    }
    
    renderer_draw_text_centered("Press ESC, H, SPACE, or ENTER to return", SCREEN_HEIGHT - 50, 24, YELLOW);
}

void renderer_draw_game_screen(const Map* current_map, Vector2 player_position, bool invincible, int invincibility_timer,
                               float health, float max_health, int current_map_id, int coins_collected) {
    if (!current_map) return;
    
    renderer_draw_map(current_map);
    
    int coin_count;
    const Coin* coins = map_get_coins(current_map, &coin_count);
    for (int i = 0; i < coin_count; i++) {
        renderer_draw_coin(coins[i].position, coins[i].collected);
    }
    
    int obstacle_count;
    const Obstacle* obstacles = map_get_obstacles(current_map, &obstacle_count);
    for (int i = 0; i < obstacle_count; i++) {
        renderer_draw_obstacle(obstacles[i].position, obstacles[i].radius, obstacles[i].color);
    }
    
    renderer_draw_health_bar(SCREEN_WIDTH - 220, 20, 200, 20, health, max_health);
    
    renderer_draw_text("WASD to move", 10, 10, 20, BLACK);
    char map_text[50];
    snprintf(map_text, sizeof(map_text), "Map: %d", current_map_id);
    renderer_draw_text(map_text, 10, 35, 20, BLACK);
    char coin_text[50];
    snprintf(coin_text, sizeof(coin_text), "Coins: %d", coins_collected);
    renderer_draw_text(coin_text, 10, 60, 20, GOLD);
    char pos_text[100];
    snprintf(pos_text, sizeof(pos_text), "Position: (%.0f, %.0f)", player_position.x, player_position.y);
    renderer_draw_text(pos_text, 10, 85, 20, BLACK);
    renderer_draw_fps(10, 110);
    
    renderer_draw_player(player_position, invincible, invincibility_timer);
}
