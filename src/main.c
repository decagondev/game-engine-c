#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Map structures
typedef struct {
    Rectangle rect;
} Wall;

typedef struct {
    Rectangle rect;
    int target_map_id;
    int target_entrance_id;
} Exit;

typedef struct {
    Vector2 position;
} Entrance;

typedef struct {
    Vector2 position;
    bool collected;
} Coin;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    int direction_change_timer;
    Color color;
} Obstacle;

#define MAX_WALLS 20
#define MAX_EXITS 4
#define MAX_ENTRANCES 4
#define MAX_COINS 10
#define MAX_OBSTACLES 5
#define NUM_MAPS 4
#define COIN_RADIUS 15.0f
#define OBSTACLE_RADIUS 20.0f
#define OBSTACLE_SPEED 2.0f
#define OBSTACLE_DIRECTION_CHANGE_FRAMES 120  // Change direction every 2 seconds at 60fps

typedef struct {
    int map_id;
    Wall walls[MAX_WALLS];
    int wall_count;
    Exit exits[MAX_EXITS];
    int exit_count;
    Entrance entrances[MAX_ENTRANCES];
    int entrance_count;
    Coin coins[MAX_COINS];
    int coin_count;
    Obstacle obstacles[MAX_OBSTACLES];
    int obstacle_count;
    Color bg_color;
} Map;

// High score structure
#define MAX_HIGH_SCORES 10
#define HIGH_SCORE_FILENAME "highscores.txt"
#define MAX_NAME_LENGTH 20

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int frame_count;
    int coins_collected;
    float health_remaining;
} HighScore;

// Game screen states
typedef enum {
    GAME_STATE_START,
    GAME_STATE_PLAYING,
    GAME_STATE_END,
    GAME_STATE_ENTER_NAME,
    GAME_STATE_HIGH_SCORES
} GameStateType;

// Game state structure
typedef struct {
    bool running;
    int frame_count;
    int game_start_frame;  // Frame count when game started (for score calculation)
    Vector2 position;  // Player position
    float speed;       // Movement speed
    int current_map_id;
    int coins_collected;
    int total_coins;
    float health;
    float max_health;
    int invincibility_timer;
    GameStateType state;
    Map maps[NUM_MAPS];
    HighScore high_scores[MAX_HIGH_SCORES];
    int high_score_count;
    char player_name[MAX_NAME_LENGTH + 1];
    int name_char_count;
    HighScore pending_score;  // Score waiting to be saved with name
} GameState;

// Window settings
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_TITLE "Game Engine"
#define PLAYER_SPEED 5.0f
#define PLAYER_RADIUS 25.0f
#define EXIT_WIDTH 60.0f
#define EXIT_HEIGHT 60.0f
#define MAX_HEALTH 100.0f
#define DAMAGE_PER_HIT 10.0f
#define INVINCIBILITY_FRAMES 60  // 1 second of invincibility at 60fps

// Helper function to check circle-rectangle collision
bool CheckCircleRectCollision(Vector2 circle_pos, float radius, Rectangle rect) {
    float closest_x = fmaxf(rect.x, fminf(circle_pos.x, rect.x + rect.width));
    float closest_y = fmaxf(rect.y, fminf(circle_pos.y, rect.y + rect.height));
    
    float distance_x = circle_pos.x - closest_x;
    float distance_y = circle_pos.y - closest_y;
    
    return (distance_x * distance_x + distance_y * distance_y) < (radius * radius);
}

// Check if a position is valid (not colliding with walls)
bool IsValidSpawnPosition(Vector2 position, float radius, Map* map) {
    // Check screen bounds
    if (position.x < radius || position.x > SCREEN_WIDTH - radius ||
        position.y < radius || position.y > SCREEN_HEIGHT - radius) {
        return false;
    }
    
    // Check wall collisions
    for (int i = 0; i < map->wall_count; i++) {
        if (CheckCircleRectCollision(position, radius, map->walls[i].rect)) {
            return false;
        }
    }
    
    return true;
}

// Find a valid spawn position near the desired position
Vector2 FindValidSpawnPosition(Vector2 desired_pos, float radius, Map* map) {
    // If desired position is valid, use it
    if (IsValidSpawnPosition(desired_pos, radius, map)) {
        return desired_pos;
    }
    
    // Try positions in a spiral pattern around the desired position
    float search_radius = radius * 2;
    int max_attempts = 50;
    
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        // Try positions in a circle around desired position
        for (int angle_step = 0; angle_step < 8; angle_step++) {
            float angle = (angle_step * 45.0f) * DEG2RAD;
            Vector2 test_pos = {
                desired_pos.x + cosf(angle) * search_radius,
                desired_pos.y + sinf(angle) * search_radius
            };
            
            if (IsValidSpawnPosition(test_pos, radius, map)) {
                return test_pos;
            }
        }
        
        // Increase search radius
        search_radius += radius;
    }
    
    // Fallback: return a safe default position (center of screen)
    Vector2 safe_pos = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    return safe_pos;
}

// Initialize a map
void init_map(Map* map, int map_id) {
    map->map_id = map_id;
    map->wall_count = 0;
    map->exit_count = 0;
    map->entrance_count = 0;
    map->coin_count = 0;
    map->obstacle_count = 0;
    
    // Set background color based on map
    switch(map_id) {
        case 0: map->bg_color = (Color){240, 240, 255, 255}; break; // Light blue
        case 1: map->bg_color = (Color){255, 240, 240, 255}; break; // Light red
        case 2: map->bg_color = (Color){240, 255, 240, 255}; break; // Light green
        case 3: map->bg_color = (Color){255, 255, 240, 255}; break; // Light yellow
        default: map->bg_color = RAYWHITE; break;
    }
    
    // Map 0: Top-left room
    if (map_id == 0) {
        // Walls - create open layout with obstacles
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 50, 150, 20}}; // Top-left wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){250, 50, 150, 20}}; // Top-right wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 50, 20, 150}}; // Left-top wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 250, 20, 150}}; // Left-bottom wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){200, 200, 100, 20}}; // Center horizontal wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){200, 200, 20, 100}}; // Center vertical wall
        
        // Exit to Map 1 (right)
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH - EXIT_WIDTH - 20, SCREEN_HEIGHT/2 - EXIT_HEIGHT/2, EXIT_WIDTH, EXIT_HEIGHT}, 1, 0};
        // Exit to Map 2 (bottom)
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH/2 - EXIT_WIDTH/2, SCREEN_HEIGHT - EXIT_HEIGHT - 20, EXIT_WIDTH, EXIT_HEIGHT}, 2, 0};
        
        // Entrances
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}}; // Default spawn
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){50, SCREEN_HEIGHT/2}}; // From Map 1 (left)
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, 50}}; // From Map 2 (top)
        
        // Coins - placed in accessible areas
        map->coins[map->coin_count++] = (Coin){(Vector2){150, 150}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){350, 200}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){150, 350}, false};
        
        // Obstacles
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){300, 250}, (Vector2){OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 0, RED};
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){200, 300}, (Vector2){-OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 60, RED};
        
        // Validate and adjust spawn positions
        for (int i = 0; i < map->entrance_count; i++) {
            Vector2 valid_pos = FindValidSpawnPosition(map->entrances[i].position, PLAYER_RADIUS, map);
            if (valid_pos.x != map->entrances[i].position.x || valid_pos.y != map->entrances[i].position.y) {
                printf("Map %d: Adjusted entrance %d from (%.0f, %.0f) to (%.0f, %.0f)\n", 
                       map_id, i, map->entrances[i].position.x, map->entrances[i].position.y, valid_pos.x, valid_pos.y);
            }
            map->entrances[i].position = valid_pos;
        }
        
        for (int i = 0; i < map->obstacle_count; i++) {
            Vector2 valid_pos = FindValidSpawnPosition(map->obstacles[i].position, OBSTACLE_RADIUS, map);
            if (valid_pos.x != map->obstacles[i].position.x || valid_pos.y != map->obstacles[i].position.y) {
                printf("Map %d: Adjusted obstacle %d from (%.0f, %.0f) to (%.0f, %.0f)\n", 
                       map_id, i, map->obstacles[i].position.x, map->obstacles[i].position.y, valid_pos.x, valid_pos.y);
            }
            map->obstacles[i].position = valid_pos;
        }
    }
    // Map 1: Top-right room
    else if (map_id == 1) {
        // Walls - create open layout with obstacles
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 50, 150, 20}}; // Top-left wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){650, 50, 150, 20}}; // Top-right wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 50, 20, 150}}; // Left-top wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 250, 20, 150}}; // Left-bottom wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){600, 200, 100, 20}}; // Center horizontal wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){600, 200, 20, 100}}; // Center vertical wall
        
        // Exit to Map 0 (left)
        map->exits[map->exit_count++] = (Exit){(Rectangle){20, SCREEN_HEIGHT/2 - EXIT_HEIGHT/2, EXIT_WIDTH, EXIT_HEIGHT}, 0, 1};
        // Exit to Map 3 (bottom)
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH/2 - EXIT_WIDTH/2, SCREEN_HEIGHT - EXIT_HEIGHT - 20, EXIT_WIDTH, EXIT_HEIGHT}, 3, 0};
        
        // Entrances
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}}; // Default spawn
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH - 50, SCREEN_HEIGHT/2}}; // From Map 0 (right)
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, 50}}; // From Map 3 (top)
        
        // Coins - placed in accessible areas
        map->coins[map->coin_count++] = (Coin){(Vector2){550, 150}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){750, 200}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){550, 350}, false};
        
        // Obstacles
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){600, 250}, (Vector2){OBSTACLE_SPEED, -OBSTACLE_SPEED}, OBSTACLE_RADIUS, 30, RED};
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){700, 300}, (Vector2){-OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 90, RED};
        
        // Validate and adjust spawn positions
        for (int i = 0; i < map->entrance_count; i++) {
            Vector2 valid_pos = FindValidSpawnPosition(map->entrances[i].position, PLAYER_RADIUS, map);
            if (valid_pos.x != map->entrances[i].position.x || valid_pos.y != map->entrances[i].position.y) {
                printf("Map %d: Adjusted entrance %d from (%.0f, %.0f) to (%.0f, %.0f)\n", 
                       map_id, i, map->entrances[i].position.x, map->entrances[i].position.y, valid_pos.x, valid_pos.y);
            }
            map->entrances[i].position = valid_pos;
        }
        
        for (int i = 0; i < map->obstacle_count; i++) {
            Vector2 valid_pos = FindValidSpawnPosition(map->obstacles[i].position, OBSTACLE_RADIUS, map);
            if (valid_pos.x != map->obstacles[i].position.x || valid_pos.y != map->obstacles[i].position.y) {
                printf("Map %d: Adjusted obstacle %d from (%.0f, %.0f) to (%.0f, %.0f)\n", 
                       map_id, i, map->obstacles[i].position.x, map->obstacles[i].position.y, valid_pos.x, valid_pos.y);
            }
            map->obstacles[i].position = valid_pos;
        }
    }
    // Map 2: Bottom-left room
    else if (map_id == 2) {
        // Walls - create open layout with obstacles
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 400, 150, 20}}; // Top-left wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){250, 400, 150, 20}}; // Top-right wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 400, 20, 150}}; // Left-top wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 600, 20, 150}}; // Left-bottom wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){200, 550, 100, 20}}; // Center horizontal wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){200, 550, 20, 100}}; // Center vertical wall
        
        // Exit to Map 0 (top)
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH/2 - EXIT_WIDTH/2, 20, EXIT_WIDTH, EXIT_HEIGHT}, 0, 2};
        // Exit to Map 3 (right)
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH - EXIT_WIDTH - 20, SCREEN_HEIGHT/2 - EXIT_HEIGHT/2, EXIT_WIDTH, EXIT_HEIGHT}, 3, 1};
        
        // Entrances
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}}; // Default spawn
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT - 50}}; // From Map 0 (bottom)
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){50, SCREEN_HEIGHT/2}}; // From Map 3 (left)
        
        // Coins - placed in accessible areas
        map->coins[map->coin_count++] = (Coin){(Vector2){150, 500}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){350, 450}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){150, 350}, false};
        
        // Obstacles
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){300, 500}, (Vector2){OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 45, RED};
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){200, 450}, (Vector2){-OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 120, RED};
        
        // Validate and adjust spawn positions
        for (int i = 0; i < map->entrance_count; i++) {
            Vector2 valid_pos = FindValidSpawnPosition(map->entrances[i].position, PLAYER_RADIUS, map);
            if (valid_pos.x != map->entrances[i].position.x || valid_pos.y != map->entrances[i].position.y) {
                printf("Map %d: Adjusted entrance %d from (%.0f, %.0f) to (%.0f, %.0f)\n", 
                       map_id, i, map->entrances[i].position.x, map->entrances[i].position.y, valid_pos.x, valid_pos.y);
            }
            map->entrances[i].position = valid_pos;
        }
        
        for (int i = 0; i < map->obstacle_count; i++) {
            Vector2 valid_pos = FindValidSpawnPosition(map->obstacles[i].position, OBSTACLE_RADIUS, map);
            if (valid_pos.x != map->obstacles[i].position.x || valid_pos.y != map->obstacles[i].position.y) {
                printf("Map %d: Adjusted obstacle %d from (%.0f, %.0f) to (%.0f, %.0f)\n", 
                       map_id, i, map->obstacles[i].position.x, map->obstacles[i].position.y, valid_pos.x, valid_pos.y);
            }
            map->obstacles[i].position = valid_pos;
        }
    }
    // Map 3: Bottom-right room
    else if (map_id == 3) {
        // Walls - create open layout with obstacles
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 400, 150, 20}}; // Top-left wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){650, 400, 150, 20}}; // Top-right wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 400, 20, 150}}; // Left-top wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 600, 20, 150}}; // Left-bottom wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){600, 550, 100, 20}}; // Center horizontal wall
        map->walls[map->wall_count++] = (Wall){(Rectangle){600, 550, 20, 100}}; // Center vertical wall
        
        // Exit to Map 1 (top)
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH/2 - EXIT_WIDTH/2, 20, EXIT_WIDTH, EXIT_HEIGHT}, 1, 2};
        // Exit to Map 2 (left)
        map->exits[map->exit_count++] = (Exit){(Rectangle){20, SCREEN_HEIGHT/2 - EXIT_HEIGHT/2, EXIT_WIDTH, EXIT_HEIGHT}, 2, 2};
        
        // Entrances
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}}; // Default spawn
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT - 50}}; // From Map 1 (bottom)
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH - 50, SCREEN_HEIGHT/2}}; // From Map 2 (right)
        
        // Coins - placed in accessible areas
        map->coins[map->coin_count++] = (Coin){(Vector2){550, 500}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){750, 450}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){550, 350}, false};
        
        // Obstacles
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){600, 500}, (Vector2){OBSTACLE_SPEED, -OBSTACLE_SPEED}, OBSTACLE_RADIUS, 75, RED};
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){700, 450}, (Vector2){-OBSTACLE_SPEED, -OBSTACLE_SPEED}, OBSTACLE_RADIUS, 15, RED};
        
        // Validate and adjust spawn positions
        for (int i = 0; i < map->entrance_count; i++) {
            Vector2 valid_pos = FindValidSpawnPosition(map->entrances[i].position, PLAYER_RADIUS, map);
            if (valid_pos.x != map->entrances[i].position.x || valid_pos.y != map->entrances[i].position.y) {
                printf("Map %d: Adjusted entrance %d from (%.0f, %.0f) to (%.0f, %.0f)\n", 
                       map_id, i, map->entrances[i].position.x, map->entrances[i].position.y, valid_pos.x, valid_pos.y);
            }
            map->entrances[i].position = valid_pos;
        }
        
        for (int i = 0; i < map->obstacle_count; i++) {
            Vector2 valid_pos = FindValidSpawnPosition(map->obstacles[i].position, OBSTACLE_RADIUS, map);
            if (valid_pos.x != map->obstacles[i].position.x || valid_pos.y != map->obstacles[i].position.y) {
                printf("Map %d: Adjusted obstacle %d from (%.0f, %.0f) to (%.0f, %.0f)\n", 
                       map_id, i, map->obstacles[i].position.x, map->obstacles[i].position.y, valid_pos.x, valid_pos.y);
            }
            map->obstacles[i].position = valid_pos;
        }
    }
}

// Check if all coins are collected
bool all_coins_collected(GameState* game) {
    int total_collected = 0;
    for (int i = 0; i < NUM_MAPS; i++) {
        for (int j = 0; j < game->maps[i].coin_count; j++) {
            if (game->maps[i].coins[j].collected) {
                total_collected++;
            }
        }
    }
    return total_collected >= game->total_coins;
}

// Load high scores from file
void load_high_scores(GameState* game) {
    game->high_score_count = 0;
    
    FILE* file = fopen(HIGH_SCORE_FILENAME, "r");
    if (file == NULL) {
        // File doesn't exist yet, that's okay
        return;
    }
    
    HighScore score;
    char line[256];
    
    while (game->high_score_count < MAX_HIGH_SCORES && fgets(line, sizeof(line), file) != NULL) {
        // Parse line: name frame_count coins_collected health_remaining
        char name_buffer[MAX_NAME_LENGTH + 1] = {0};
        if (sscanf(line, "%20s %d %d %f", name_buffer, &score.frame_count, &score.coins_collected, &score.health_remaining) == 4) {
            strncpy(score.name, name_buffer, MAX_NAME_LENGTH);
            score.name[MAX_NAME_LENGTH] = '\0';
            game->high_scores[game->high_score_count++] = score;
        }
    }
    
    fclose(file);
}

// Save high scores to file
void save_high_scores(GameState* game) {
    FILE* file = fopen(HIGH_SCORE_FILENAME, "w");
    if (file == NULL) {
        printf("Error: Could not save high scores to file\n");
        return;
    }
    
    for (int i = 0; i < game->high_score_count; i++) {
        fprintf(file, "%s %d %d %.1f\n", 
                game->high_scores[i].name,
                game->high_scores[i].frame_count,
                game->high_scores[i].coins_collected,
                game->high_scores[i].health_remaining);
    }
    
    fclose(file);
}

// Add a new high score (if it qualifies)
void add_high_score(GameState* game, const char* name, int frame_count, int coins_collected, float health_remaining) {
    HighScore new_score;
    strncpy(new_score.name, name, MAX_NAME_LENGTH);
    new_score.name[MAX_NAME_LENGTH] = '\0';
    new_score.frame_count = frame_count;
    new_score.coins_collected = coins_collected;
    new_score.health_remaining = health_remaining;
    
    // Find insertion point (scores are sorted by frame_count, lowest first)
    int insert_pos = game->high_score_count;
    for (int i = 0; i < game->high_score_count; i++) {
        if (frame_count < game->high_scores[i].frame_count) {
            insert_pos = i;
            break;
        }
    }
    
    // If we have space or this is better than the worst score
    if (game->high_score_count < MAX_HIGH_SCORES || insert_pos < MAX_HIGH_SCORES) {
        // Shift scores down if needed
        if (game->high_score_count >= MAX_HIGH_SCORES) {
            // Remove worst score
            game->high_score_count = MAX_HIGH_SCORES - 1;
        }
        
        // Shift scores to make room
        for (int i = game->high_score_count; i > insert_pos; i--) {
            game->high_scores[i] = game->high_scores[i - 1];
        }
        
        // Insert new score
        game->high_scores[insert_pos] = new_score;
        game->high_score_count++;
        
        // Save to file
        save_high_scores(game);
        printf("New high score added! Rank: %d, Name: %s, Frames: %d\n", insert_pos + 1, name, frame_count);
    }
}

// Initialize game
void game_init(GameState* game) {
    // Initialize raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60); // Set to run at 60 frames-per-second
    
    game->running = true;
    game->frame_count = 0;
    game->game_start_frame = 0;
    game->speed = PLAYER_SPEED;
    game->current_map_id = 0;
    game->coins_collected = 0;
    game->state = GAME_STATE_START;
    game->health = MAX_HEALTH;
    game->max_health = MAX_HEALTH;
    game->invincibility_timer = 0;
    game->high_score_count = 0;
    memset(game->player_name, 0, sizeof(game->player_name));
    game->name_char_count = 0;
    memset(&game->pending_score, 0, sizeof(game->pending_score));
    
    // Load high scores from file
    load_high_scores(game);
    
    // Initialize all maps
    for (int i = 0; i < NUM_MAPS; i++) {
        init_map(&game->maps[i], i);
    }
    
    // Calculate total coins
    game->total_coins = 0;
    for (int i = 0; i < NUM_MAPS; i++) {
        game->total_coins += game->maps[i].coin_count;
    }
    
    // Start player at entrance 0 of map 0
    game->position = game->maps[0].entrances[0].position;
    
    printf("Game initialized! Window created: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    printf("Total coins: %d\n", game->total_coins);
    printf("High scores loaded: %d\n", game->high_score_count);
}

// Update game logic
void game_update(GameState* game) {
    game->frame_count++;
    
    // Check for window close button or ESC key
    if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
        game->running = false;
    }
    
    // Handle state transitions
    if (game->state == GAME_STATE_START) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            game->state = GAME_STATE_PLAYING;
            game->game_start_frame = game->frame_count; // Track when game starts
        }
        if (IsKeyPressed(KEY_H)) {
            game->state = GAME_STATE_HIGH_SCORES;
        }
        return;
    }
    
    if (game->state == GAME_STATE_HIGH_SCORES) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_H) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            game->state = GAME_STATE_START;
        }
        return;
    }
    
    if (game->state == GAME_STATE_ENTER_NAME) {
        // Handle text input
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (game->name_char_count < MAX_NAME_LENGTH)) {
                game->player_name[game->name_char_count] = (char)key;
                game->name_char_count++;
                game->player_name[game->name_char_count] = '\0';
            }
            key = GetCharPressed();
        }
        
        // Handle backspace
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (game->name_char_count > 0) {
                game->name_char_count--;
                game->player_name[game->name_char_count] = '\0';
            }
        }
        
        // Submit name
        if (IsKeyPressed(KEY_ENTER)) {
            if (game->name_char_count > 0) {
                // Add high score with name
                add_high_score(game, game->player_name, 
                              game->pending_score.frame_count,
                              game->pending_score.coins_collected,
                              game->pending_score.health_remaining);
                game->state = GAME_STATE_END;
            } else {
                // Use default name if empty
                add_high_score(game, "Player", 
                              game->pending_score.frame_count,
                              game->pending_score.coins_collected,
                              game->pending_score.health_remaining);
                game->state = GAME_STATE_END;
            }
        }
        return;
    }
    
    if (game->state == GAME_STATE_END) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            // Restart game
            game->state = GAME_STATE_START;
            game->coins_collected = 0;
            game->current_map_id = 0;
            game->position = game->maps[0].entrances[0].position;
            game->health = MAX_HEALTH;
            game->invincibility_timer = 0;
            game->game_start_frame = 0;
            // Reset all coins
            for (int i = 0; i < NUM_MAPS; i++) {
                for (int j = 0; j < game->maps[i].coin_count; j++) {
                    game->maps[i].coins[j].collected = false;
                }
            }
        }
        return;
    }
    
    // Only update game logic when playing
    Map* current_map = &game->maps[game->current_map_id];
    
    // Update invincibility timer
    if (game->invincibility_timer > 0) {
        game->invincibility_timer--;
    }
    
    // Update obstacles
    for (int i = 0; i < current_map->obstacle_count; i++) {
        Obstacle* obstacle = &current_map->obstacles[i];
        
        // Update direction change timer
        obstacle->direction_change_timer++;
        if (obstacle->direction_change_timer >= OBSTACLE_DIRECTION_CHANGE_FRAMES) {
            // Randomly change direction
            float angle = (float)(GetRandomValue(0, 360)) * DEG2RAD;
            obstacle->velocity.x = cosf(angle) * OBSTACLE_SPEED;
            obstacle->velocity.y = sinf(angle) * OBSTACLE_SPEED;
            obstacle->direction_change_timer = 0;
        }
        
        // Calculate new position
        Vector2 new_obstacle_pos = {
            obstacle->position.x + obstacle->velocity.x,
            obstacle->position.y + obstacle->velocity.y
        };
        
        // Check wall collisions for obstacles
        bool can_move_obstacle = true;
        Rectangle obstacle_rect = {
            new_obstacle_pos.x - obstacle->radius,
            new_obstacle_pos.y - obstacle->radius,
            obstacle->radius * 2,
            obstacle->radius * 2
        };
        
        for (int j = 0; j < current_map->wall_count; j++) {
            if (CheckCollisionRecs(obstacle_rect, current_map->walls[j].rect)) {
                can_move_obstacle = false;
                // Bounce off wall by reversing velocity
                obstacle->velocity.x = -obstacle->velocity.x;
                obstacle->velocity.y = -obstacle->velocity.y;
                break;
            }
        }
        
        // Keep obstacle within screen bounds
        if (new_obstacle_pos.x < obstacle->radius) {
            new_obstacle_pos.x = obstacle->radius;
            obstacle->velocity.x = -obstacle->velocity.x;
        }
        if (new_obstacle_pos.x > SCREEN_WIDTH - obstacle->radius) {
            new_obstacle_pos.x = SCREEN_WIDTH - obstacle->radius;
            obstacle->velocity.x = -obstacle->velocity.x;
        }
        if (new_obstacle_pos.y < obstacle->radius) {
            new_obstacle_pos.y = obstacle->radius;
            obstacle->velocity.y = -obstacle->velocity.y;
        }
        if (new_obstacle_pos.y > SCREEN_HEIGHT - obstacle->radius) {
            new_obstacle_pos.y = SCREEN_HEIGHT - obstacle->radius;
            obstacle->velocity.y = -obstacle->velocity.y;
        }
        
        if (can_move_obstacle) {
            obstacle->position = new_obstacle_pos;
        }
    }
    
    // WASD movement input
    Vector2 movement = {0.0f, 0.0f};
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        movement.y -= game->speed;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        movement.y += game->speed;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        movement.x -= game->speed;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        movement.x += game->speed;
    }
    
    // Normalize diagonal movement to maintain consistent speed
    float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
    if (length > 0.0f) {
        movement.x = (movement.x / length) * game->speed;
        movement.y = (movement.y / length) * game->speed;
    }
    
    // Calculate new position
    Vector2 new_position = {
        game->position.x + movement.x,
        game->position.y + movement.y
    };
    
    // Check wall collisions
    bool can_move = true;
    for (int i = 0; i < current_map->wall_count; i++) {
        if (CheckCircleRectCollision(new_position, PLAYER_RADIUS, current_map->walls[i].rect)) {
            can_move = false;
            break;
        }
    }
    
    // Update position if no collision
    if (can_move) {
        game->position = new_position;
    }
    
    // Keep player within screen bounds
    if (game->position.x < PLAYER_RADIUS) {
        game->position.x = PLAYER_RADIUS;
    }
    if (game->position.x > SCREEN_WIDTH - PLAYER_RADIUS) {
        game->position.x = SCREEN_WIDTH - PLAYER_RADIUS;
    }
    if (game->position.y < PLAYER_RADIUS) {
        game->position.y = PLAYER_RADIUS;
    }
    if (game->position.y > SCREEN_HEIGHT - PLAYER_RADIUS) {
        game->position.y = SCREEN_HEIGHT - PLAYER_RADIUS;
    }
    
    // Check for exit collision
    for (int i = 0; i < current_map->exit_count; i++) {
        Exit* exit = &current_map->exits[i];
        if (CheckCircleRectCollision(game->position, PLAYER_RADIUS, exit->rect)) {
            // Switch to target map
            game->current_map_id = exit->target_map_id;
            Map* target_map = &game->maps[exit->target_map_id];
            
            // Move player to target entrance
            if (exit->target_entrance_id < target_map->entrance_count) {
                game->position = target_map->entrances[exit->target_entrance_id].position;
            } else {
                game->position = target_map->entrances[0].position;
            }
            
            printf("Entered map %d\n", exit->target_map_id);
            break;
        }
    }
    
    // Check for obstacle collision (damage)
    for (int i = 0; i < current_map->obstacle_count; i++) {
        Obstacle* obstacle = &current_map->obstacles[i];
        float distance = sqrtf((game->position.x - obstacle->position.x) * (game->position.x - obstacle->position.x) +
                               (game->position.y - obstacle->position.y) * (game->position.y - obstacle->position.y));
        if (distance < PLAYER_RADIUS + obstacle->radius && game->invincibility_timer == 0) {
            // Take damage
            game->health -= DAMAGE_PER_HIT;
            game->invincibility_timer = INVINCIBILITY_FRAMES;
            printf("Hit! Health: %.0f/%.0f\n", game->health, game->max_health);
            
            // Check if player died
            if (game->health <= 0) {
                game->health = 0;
                printf("Player died! Resetting game...\n");
                game->coins_collected = 0;
                game->current_map_id = 0;
                game->position = game->maps[0].entrances[0].position;
                game->health = MAX_HEALTH;
                game->invincibility_timer = 0;
                game->game_start_frame = game->frame_count; // Reset timer for new attempt
                // Reset all coins
                for (int j = 0; j < NUM_MAPS; j++) {
                    for (int k = 0; k < game->maps[j].coin_count; k++) {
                        game->maps[j].coins[k].collected = false;
                    }
                }
                // Reset all obstacles to initial positions
                for (int j = 0; j < NUM_MAPS; j++) {
                    init_map(&game->maps[j], j);
                }
                break;
            }
        }
    }
    
    // Check for coin collection
    for (int i = 0; i < current_map->coin_count; i++) {
        Coin* coin = &current_map->coins[i];
        if (!coin->collected) {
            float distance = sqrtf((game->position.x - coin->position.x) * (game->position.x - coin->position.x) +
                                   (game->position.y - coin->position.y) * (game->position.y - coin->position.y));
            if (distance < PLAYER_RADIUS + COIN_RADIUS) {
                coin->collected = true;
                game->coins_collected++;
                printf("Coin collected! Total: %d/%d\n", game->coins_collected, game->total_coins);
                
                // Check if all coins are collected
                if (all_coins_collected(game)) {
                    // Calculate completion frame count
                    int completion_frames = game->frame_count - game->game_start_frame;
                    printf("All coins collected! Game complete in %d frames!\n", completion_frames);
                    
                    // Store pending score and go to name entry
                    game->pending_score.frame_count = completion_frames;
                    game->pending_score.coins_collected = game->coins_collected;
                    game->pending_score.health_remaining = game->health;
                    
                    // Reset name input
                    memset(game->player_name, 0, sizeof(game->player_name));
                    game->name_char_count = 0;
                    
                    game->state = GAME_STATE_ENTER_NAME;
                }
            }
        }
    }
}

// Render start screen
void render_start_screen(GameState* game) {
    BeginDrawing();
    
    // Gradient background
    ClearBackground((Color){30, 30, 50, 255});
    
    // Title
    const char* title = "COIN COLLECTOR";
    int title_font_size = 60;
    int title_width = MeasureText(title, title_font_size);
    DrawText(title, SCREEN_WIDTH/2 - title_width/2, 150, title_font_size, GOLD);
    
    // Instructions
    const char* instruction1 = "Collect all coins across 4 maps!";
    const char* instruction2 = "Use WASD to move";
    const char* instruction3 = "Walk into green exits to change maps";
    const char* instruction4 = "Avoid red obstacles - they will kill you!";
    const char* press_key = "Press SPACE or ENTER to start";
    const char* high_score_key = "Press H to view high scores";
    
    DrawText(instruction1, SCREEN_WIDTH/2 - MeasureText(instruction1, 24)/2, 240, 24, WHITE);
    DrawText(instruction2, SCREEN_WIDTH/2 - MeasureText(instruction2, 20)/2, 280, 20, LIGHTGRAY);
    DrawText(instruction3, SCREEN_WIDTH/2 - MeasureText(instruction3, 20)/2, 310, 20, LIGHTGRAY);
    DrawText(instruction4, SCREEN_WIDTH/2 - MeasureText(instruction4, 20)/2, 340, 20, RED);
    DrawText(high_score_key, SCREEN_WIDTH/2 - MeasureText(high_score_key, 18)/2, 380, 18, YELLOW);
    
    // Blinking start text
    if ((game->frame_count / 30) % 2 == 0) {
        int press_width = MeasureText(press_key, 28);
        DrawText(press_key, SCREEN_WIDTH/2 - press_width/2, 450, 28, YELLOW);
    }
    
    // Draw some decorative coins
    for (int i = 0; i < 5; i++) {
        float x = 150 + i * 125;
        float y = 500;
        DrawCircleV((Vector2){x, y}, COIN_RADIUS, GOLD);
        DrawCircleV((Vector2){x, y}, COIN_RADIUS - 2, YELLOW);
        DrawCircleLinesV((Vector2){x, y}, COIN_RADIUS, ORANGE);
    }
    
    // Draw high scores preview
    if (game->high_score_count > 0) {
        DrawText("TOP SCORES (Press H for full list)", SCREEN_WIDTH/2 - MeasureText("TOP SCORES (Press H for full list)", 18)/2, 420, 18, GOLD);
        int y_offset = 445;
        int max_display = (game->high_score_count < 3) ? game->high_score_count : 3;
        for (int i = 0; i < max_display; i++) {
            char score_text[120];
            snprintf(score_text, sizeof(score_text), "%d. %s - %d frames", 
                     i + 1, game->high_scores[i].name, game->high_scores[i].frame_count);
            DrawText(score_text, SCREEN_WIDTH/2 - MeasureText(score_text, 16)/2, y_offset, 16, WHITE);
            y_offset += 18;
        }
    }
    
    EndDrawing();
}

// Render end screen
void render_end_screen(GameState* game) {
    BeginDrawing();
    
    // Victory background
    ClearBackground((Color){20, 50, 20, 255});
    
    // Victory message
    const char* victory = "VICTORY!";
    int victory_font_size = 70;
    int victory_width = MeasureText(victory, victory_font_size);
    DrawText(victory, SCREEN_WIDTH/2 - victory_width/2, 150, victory_font_size, GOLD);
    
    // Stats
    char stats[100];
    snprintf(stats, sizeof(stats), "You collected all %d coins!", game->total_coins);
    int stats_width = MeasureText(stats, 32);
    DrawText(stats, SCREEN_WIDTH/2 - stats_width/2, 220, 32, WHITE);
    
    int completion_frames = game->frame_count - game->game_start_frame;
    char frames[100];
    snprintf(frames, sizeof(frames), "Completion time: %d frames", completion_frames);
    int frames_width = MeasureText(frames, 24);
    DrawText(frames, SCREEN_WIDTH/2 - frames_width/2, 260, 24, LIGHTGRAY);
    
    char health_text[100];
    snprintf(health_text, sizeof(health_text), "Health remaining: %.0f/%.0f", game->health, game->max_health);
    int health_width = MeasureText(health_text, 24);
    DrawText(health_text, SCREEN_WIDTH/2 - health_width/2, 290, 24, LIGHTGRAY);
    
    // Draw high scores
    if (game->high_score_count > 0) {
        DrawText("HIGH SCORES (Lowest frames = Best)", SCREEN_WIDTH/2 - MeasureText("HIGH SCORES (Lowest frames = Best)", 20)/2, 330, 20, GOLD);
        int y_offset = 355;
        int max_display = (game->high_score_count < 5) ? game->high_score_count : 5;
        for (int i = 0; i < max_display; i++) {
            char score_text[150];
            snprintf(score_text, sizeof(score_text), "%d. %s - %d frames | Coins: %d | HP: %.0f", 
                     i + 1, game->high_scores[i].name,
                     game->high_scores[i].frame_count, 
                     game->high_scores[i].coins_collected, 
                     game->high_scores[i].health_remaining);
            DrawText(score_text, SCREEN_WIDTH/2 - MeasureText(score_text, 16)/2, y_offset, 16, WHITE);
            y_offset += 18;
        }
    }
    
    // Restart instruction
    const char* restart = "Press SPACE or ENTER to play again";
    const char* quit = "Press ESC to quit";
    
    // Blinking restart text
    if ((game->frame_count / 30) % 2 == 0) {
        int restart_width = MeasureText(restart, 28);
        DrawText(restart, SCREEN_WIDTH/2 - restart_width/2, 480, 28, YELLOW);
    }
    
    int quit_width = MeasureText(quit, 24);
    DrawText(quit, SCREEN_WIDTH/2 - quit_width/2, 520, 24, LIGHTGRAY);
    
    // Draw celebration coins
    for (int i = 0; i < 8; i++) {
        float angle = (game->frame_count * 2 + i * 45) * DEG2RAD;
        float radius = 100;
        float x = SCREEN_WIDTH/2 + cosf(angle) * radius;
        float y = SCREEN_HEIGHT/2 + 50 + sinf(angle) * radius;
        DrawCircleV((Vector2){x, y}, COIN_RADIUS, GOLD);
        DrawCircleV((Vector2){x, y}, COIN_RADIUS - 2, YELLOW);
        DrawCircleLinesV((Vector2){x, y}, COIN_RADIUS, ORANGE);
    }
    
    EndDrawing();
}

// Render name entry screen
void render_name_entry_screen(GameState* game) {
    BeginDrawing();
    
    ClearBackground((Color){30, 30, 50, 255});
    
    // Title
    const char* title = "ENTER YOUR NAME";
    int title_font_size = 50;
    int title_width = MeasureText(title, title_font_size);
    DrawText(title, SCREEN_WIDTH/2 - title_width/2, 200, title_font_size, GOLD);
    
    // Score info
    char score_info[150];
    snprintf(score_info, sizeof(score_info), "Frames: %d | Coins: %d | HP: %.0f", 
             game->pending_score.frame_count, 
             game->pending_score.coins_collected,
             game->pending_score.health_remaining);
    int info_width = MeasureText(score_info, 24);
    DrawText(score_info, SCREEN_WIDTH/2 - info_width/2, 280, 24, WHITE);
    
    // Name input box
    int box_width = 400;
    int box_height = 50;
    int box_x = SCREEN_WIDTH/2 - box_width/2;
    int box_y = 350;
    
    DrawRectangle(box_x, box_y, box_width, box_height, DARKGRAY);
    DrawRectangleLinesEx((Rectangle){box_x, box_y, box_width, box_height}, 3, WHITE);
    
    // Display name with cursor
    char display_text[MAX_NAME_LENGTH + 2];
    snprintf(display_text, sizeof(display_text), "%s_", game->player_name);
    int text_width = MeasureText(display_text, 32);
    DrawText(display_text, box_x + 10, box_y + 10, 32, WHITE);
    
    // Instructions
    const char* instruction = "Type your name and press ENTER";
    int inst_width = MeasureText(instruction, 20);
    DrawText(instruction, SCREEN_WIDTH/2 - inst_width/2, 420, 20, LIGHTGRAY);
    
    EndDrawing();
}

// Render high scores screen
void render_high_scores_screen(GameState* game) {
    BeginDrawing();
    
    ClearBackground((Color){20, 20, 40, 255});
    
    // Title
    const char* title = "HIGH SCORES";
    int title_font_size = 60;
    int title_width = MeasureText(title, title_font_size);
    DrawText(title, SCREEN_WIDTH/2 - title_width/2, 50, title_font_size, GOLD);
    
    const char* subtitle = "Lowest frames = Best score";
    int subtitle_width = MeasureText(subtitle, 20);
    DrawText(subtitle, SCREEN_WIDTH/2 - subtitle_width/2, 120, 20, LIGHTGRAY);
    
    // Draw all high scores
    if (game->high_score_count > 0) {
        int y_offset = 180;
        int max_display = (game->high_score_count < MAX_HIGH_SCORES) ? game->high_score_count : MAX_HIGH_SCORES;
        
        // Header
        const char* header = "Rank  Name                Frames  Coins  HP";
        DrawText(header, SCREEN_WIDTH/2 - MeasureText(header, 18)/2, y_offset, 18, YELLOW);
        y_offset += 35;
        
        for (int i = 0; i < max_display; i++) {
            char score_text[200];
            snprintf(score_text, sizeof(score_text), "%2d.   %-20s %6d  %5d  %.0f", 
                     i + 1, 
                     game->high_scores[i].name,
                     game->high_scores[i].frame_count, 
                     game->high_scores[i].coins_collected, 
                     game->high_scores[i].health_remaining);
            DrawText(score_text, SCREEN_WIDTH/2 - MeasureText(score_text, 18)/2, y_offset, 18, WHITE);
            y_offset += 25;
        }
    } else {
        const char* no_scores = "No high scores yet!";
        int no_scores_width = MeasureText(no_scores, 32);
        DrawText(no_scores, SCREEN_WIDTH/2 - no_scores_width/2, 300, 32, LIGHTGRAY);
    }
    
    // Instructions
    const char* instruction = "Press ESC, H, SPACE, or ENTER to return";
    int inst_width = MeasureText(instruction, 24);
    DrawText(instruction, SCREEN_WIDTH/2 - inst_width/2, SCREEN_HEIGHT - 50, 24, YELLOW);
    
    EndDrawing();
}

// Render game
void game_render(GameState* game) {
    // Handle different game states
    if (game->state == GAME_STATE_START) {
        render_start_screen(game);
        return;
    }
    
    if (game->state == GAME_STATE_END) {
        render_end_screen(game);
        return;
    }
    
    if (game->state == GAME_STATE_ENTER_NAME) {
        render_name_entry_screen(game);
        return;
    }
    
    if (game->state == GAME_STATE_HIGH_SCORES) {
        render_high_scores_screen(game);
        return;
    }
    
    // Render playing state
    BeginDrawing();
    
    Map* current_map = &game->maps[game->current_map_id];
    
    // Clear background with map color
    ClearBackground(current_map->bg_color);
    
    // Draw walls
    for (int i = 0; i < current_map->wall_count; i++) {
        DrawRectangleRec(current_map->walls[i].rect, DARKGRAY);
        DrawRectangleLinesEx(current_map->walls[i].rect, 2, BLACK);
    }
    
    // Draw exits
    for (int i = 0; i < current_map->exit_count; i++) {
        Exit* exit = &current_map->exits[i];
        DrawRectangleRec(exit->rect, GREEN);
        DrawRectangleLinesEx(exit->rect, 3, DARKGREEN);
        // Draw arrow indicator
        Vector2 center = {exit->rect.x + exit->rect.width/2, exit->rect.y + exit->rect.height/2};
        DrawText("→", center.x - 10, center.y - 10, 20, WHITE);
    }
    
    // Draw coins
    for (int i = 0; i < current_map->coin_count; i++) {
        Coin* coin = &current_map->coins[i];
        if (!coin->collected) {
            // Draw coin as a golden circle
            DrawCircleV(coin->position, COIN_RADIUS, GOLD);
            DrawCircleV(coin->position, COIN_RADIUS - 2, YELLOW);
            DrawCircleLinesV(coin->position, COIN_RADIUS, ORANGE);
        }
    }
    
    // Draw obstacles
    for (int i = 0; i < current_map->obstacle_count; i++) {
        Obstacle* obstacle = &current_map->obstacles[i];
        // Draw obstacle as a red circle with warning indicator
        DrawCircleV(obstacle->position, obstacle->radius, obstacle->color);
        DrawCircleV(obstacle->position, obstacle->radius - 2, MAROON);
        DrawCircleLinesV(obstacle->position, obstacle->radius, BLACK);
        // Draw X mark to indicate danger
        float x = obstacle->position.x;
        float y = obstacle->position.y;
        float size = obstacle->radius * 0.6f;
        DrawLineEx((Vector2){x - size, y - size}, (Vector2){x + size, y + size}, 3, WHITE);
        DrawLineEx((Vector2){x - size, y + size}, (Vector2){x + size, y - size}, 3, WHITE);
    }
    
    // Draw health bar
    float health_bar_width = 200.0f;
    float health_bar_height = 20.0f;
    float health_bar_x = SCREEN_WIDTH - health_bar_width - 20;
    float health_bar_y = 20;
    
    // Health bar background
    DrawRectangle(health_bar_x, health_bar_y, health_bar_width, health_bar_height, GRAY);
    DrawRectangleLinesEx((Rectangle){health_bar_x, health_bar_y, health_bar_width, health_bar_height}, 2, BLACK);
    
    // Health bar fill
    float health_percentage = game->health / game->max_health;
    if (health_percentage < 0) health_percentage = 0;
    float health_fill_width = health_bar_width * health_percentage;
    
    // Color based on health percentage
    Color health_color;
    if (health_percentage > 0.6f) {
        health_color = GREEN;
    } else if (health_percentage > 0.3f) {
        health_color = YELLOW;
    } else {
        health_color = RED;
    }
    
    DrawRectangle(health_bar_x, health_bar_y, health_fill_width, health_bar_height, health_color);
    
    // Health text
    char health_text[50];
    snprintf(health_text, sizeof(health_text), "HP: %.0f/%.0f", game->health, game->max_health);
    DrawText(health_text, health_bar_x, health_bar_y + health_bar_height + 5, 16, BLACK);
    
    // Draw UI information
    DrawText("WASD to move", 10, 10, 20, BLACK);
    DrawText(TextFormat("Map: %d", game->current_map_id), 10, 35, 20, BLACK);
    DrawText(TextFormat("Coins: %d", game->coins_collected), 10, 60, 20, GOLD);
    DrawText(TextFormat("Position: (%.0f, %.0f)", game->position.x, game->position.y), 10, 85, 20, BLACK);
    DrawFPS(10, 110);
    
    // Draw player (circle) - flash when invincible
    Color player_color = BLUE;
    Color player_inner_color = DARKBLUE;
    if (game->invincibility_timer > 0) {
        // Flash effect when invincible (every 5 frames)
        if ((game->invincibility_timer / 5) % 2 == 0) {
            player_color = (Color){player_color.r, player_color.g, player_color.b, 128}; // Semi-transparent
            player_inner_color = (Color){player_inner_color.r, player_inner_color.g, player_inner_color.b, 128};
        } else {
            player_color = (Color){player_color.r, player_color.g, player_color.b, 255};
            player_inner_color = (Color){player_inner_color.r, player_inner_color.g, player_inner_color.b, 255};
        }
    }
    
    DrawCircleV(game->position, PLAYER_RADIUS, player_color);
    DrawCircleV(game->position, PLAYER_RADIUS - 2, player_inner_color);
    
    EndDrawing();
}

// Cleanup game resources
void game_cleanup(GameState* game) {
    CloseWindow(); // Close window and OpenGL context
    printf("Game cleanup. Total frames: %d\n", game->frame_count);
}

// Main game loop
int main(void) {
    GameState game;
    
    game_init(&game);
    
    // Main game loop
    while (game.running) {
        game_update(&game);
        game_render(&game);
    }
    
    game_cleanup(&game);
    
    return 0;
}

