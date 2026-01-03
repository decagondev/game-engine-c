#include "../include/player.h"
#include "../include/map.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Screen constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Player structure (Single Responsibility - player state)
struct Player {
    Vector2 position;
    float speed;
    float health;
    float max_health;
    int invincibility_timer;
};

// Player creation
Player* player_create(void) {
    Player* player = (Player*)malloc(sizeof(Player));
    if (!player) return NULL;
    memset(player, 0, sizeof(Player));
    return player;
}

// Player destruction
void player_destroy(Player* player) {
    if (player) free(player);
}

// Player initialization
void player_init(Player* player, Vector2 start_position) {
    if (!player) return;
    player->position = start_position;
    player->speed = PLAYER_SPEED;
    player->health = MAX_HEALTH;
    player->max_health = MAX_HEALTH;
    player->invincibility_timer = 0;
}

// Player state queries
Vector2 player_get_position(const Player* player) {
    if (!player) return (Vector2){0, 0};
    return player->position;
}

float player_get_health(const Player* player) {
    if (!player) return 0.0f;
    return player->health;
}

float player_get_max_health(const Player* player) {
    if (!player) return 0.0f;
    return player->max_health;
}

bool player_is_invincible(const Player* player) {
    if (!player) return false;
    return player->invincibility_timer > 0;
}

int player_get_invincibility_timer(const Player* player) {
    if (!player) return 0;
    return player->invincibility_timer;
}

bool player_is_alive(const Player* player) {
    if (!player) return false;
    return player->health > 0;
}

// Player state modifications
void player_set_position(Player* player, Vector2 position) {
    if (!player) return;
    player->position = position;
}

void player_set_health(Player* player, float health) {
    if (!player) return;
    player->health = health;
    if (player->health < 0) player->health = 0;
    if (player->health > player->max_health) player->health = player->max_health;
}

void player_reset(Player* player, Vector2 start_position) {
    if (!player) return;
    player_init(player, start_position);
}

// Player movement
void player_handle_input(Player* player) {
    if (!player) return;
    // Input handling is done in update_movement for now
    // This can be extended for more complex input handling
}

void player_update_movement(Player* player, const struct Map* current_map) {
    if (!player || !current_map) return;
    
    // WASD movement input
    Vector2 movement = {0.0f, 0.0f};
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        movement.y -= player->speed;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        movement.y += player->speed;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        movement.x -= player->speed;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        movement.x += player->speed;
    }
    
    // Normalize diagonal movement to maintain consistent speed
    float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
    if (length > 0.0f) {
        movement.x = (movement.x / length) * player->speed;
        movement.y = (movement.y / length) * player->speed;
    }
    
    // Calculate new position
    Vector2 new_position = {
        player->position.x + movement.x,
        player->position.y + movement.y
    };
    
    // Check wall collisions
    if (!player_check_wall_collision(player, new_position, current_map)) {
        player->position = new_position;
    }
    
    // Keep player within screen bounds
    if (player->position.x < PLAYER_RADIUS) {
        player->position.x = PLAYER_RADIUS;
    }
    if (player->position.x > SCREEN_WIDTH - PLAYER_RADIUS) {
        player->position.x = SCREEN_WIDTH - PLAYER_RADIUS;
    }
    if (player->position.y < PLAYER_RADIUS) {
        player->position.y = PLAYER_RADIUS;
    }
    if (player->position.y > SCREEN_HEIGHT - PLAYER_RADIUS) {
        player->position.y = SCREEN_HEIGHT - PLAYER_RADIUS;
    }
}

// Player collision detection
bool player_check_wall_collision(const Player* player, Vector2 new_position, const struct Map* current_map) {
    if (!player || !current_map) return false;
    
    int wall_count;
    const Wall* walls = map_get_walls(current_map, &wall_count);
    for (int i = 0; i < wall_count; i++) {
        if (map_check_circle_rect_collision(new_position, PLAYER_RADIUS, walls[i].rect)) {
            return true;
        }
    }
    return false;
}

bool player_check_exit_collision(const Player* player, const struct Map* current_map, int* target_map_id, int* target_entrance_id) {
    if (!player || !current_map) return false;
    
    int exit_count;
    const Exit* exits = map_get_exits(current_map, &exit_count);
    for (int i = 0; i < exit_count; i++) {
        if (map_check_circle_rect_collision(player->position, PLAYER_RADIUS, exits[i].rect)) {
            if (target_map_id) *target_map_id = exits[i].target_map_id;
            if (target_entrance_id) *target_entrance_id = exits[i].target_entrance_id;
            return true;
        }
    }
    return false;
}

void player_apply_damage(Player* player, float damage) {
    if (!player || player->invincibility_timer > 0) return;
    
    player->health -= damage;
    player->invincibility_timer = INVINCIBILITY_FRAMES;
    
    if (player->health < 0) {
        player->health = 0;
    }
}

void player_update(Player* player) {
    if (!player) return;
    
    // Update invincibility timer
    if (player->invincibility_timer > 0) {
        player->invincibility_timer--;
    }
}

