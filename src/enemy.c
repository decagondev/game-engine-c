#include "../include/enemy.h"
#include "../include/map.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct Enemy {
    Vector2 position;
    Vector2 velocity;
    float radius;
    int direction_change_timer;
    Color color;
};

Enemy* enemy_create(Vector2 position, Vector2 velocity, Color color) {
    Enemy* enemy = (Enemy*)malloc(sizeof(Enemy));
    if (!enemy) return NULL;
    
    enemy->position = position;
    enemy->velocity = velocity;
    enemy->radius = ENEMY_RADIUS;
    enemy->direction_change_timer = 0;
    enemy->color = color;
    
    return enemy;
}

void enemy_destroy(Enemy* enemy) {
    if (enemy) free(enemy);
}

Vector2 enemy_get_position(const Enemy* enemy) {
    if (!enemy) return (Vector2){0, 0};
    return enemy->position;
}

Vector2 enemy_get_velocity(const Enemy* enemy) {
    if (!enemy) return (Vector2){0, 0};
    return enemy->velocity;
}

float enemy_get_radius(const Enemy* enemy) {
    if (!enemy) return 0.0f;
    return enemy->radius;
}

Color enemy_get_color(const Enemy* enemy) {
    if (!enemy) return BLACK;
    return enemy->color;
}

int enemy_get_direction_timer(const Enemy* enemy) {
    if (!enemy) return 0;
    return enemy->direction_change_timer;
}

void enemy_set_position(Enemy* enemy, Vector2 position) {
    if (!enemy) return;
    enemy->position = position;
}

void enemy_set_velocity(Enemy* enemy, Vector2 velocity) {
    if (!enemy) return;
    enemy->velocity = velocity;
}

void enemy_set_direction_timer(Enemy* enemy, int timer) {
    if (!enemy) return;
    enemy->direction_change_timer = timer;
}

void enemy_update(Enemy* enemy, const struct Map* current_map) {
    if (!enemy || !current_map) return;
    
    enemy->direction_change_timer++;
    if (enemy->direction_change_timer >= ENEMY_DIRECTION_CHANGE_FRAMES) {
        float angle = (float)(GetRandomValue(0, 360)) * DEG2RAD;
        enemy->velocity.x = cosf(angle) * ENEMY_SPEED;
        enemy->velocity.y = sinf(angle) * ENEMY_SPEED;
        enemy->direction_change_timer = 0;
    }
    
    Vector2 new_position = {
        enemy->position.x + enemy->velocity.x,
        enemy->position.y + enemy->velocity.y
    };
    
    if (enemy_check_wall_collision(enemy, new_position, current_map)) {
        enemy->velocity.x = -enemy->velocity.x;
        enemy->velocity.y = -enemy->velocity.y;
    } else {
        enemy->position = new_position;
    }
    
    if (enemy->position.x < enemy->radius) {
        enemy->position.x = enemy->radius;
        enemy->velocity.x = -enemy->velocity.x;
    }
    if (enemy->position.x > SCREEN_WIDTH - enemy->radius) {
        enemy->position.x = SCREEN_WIDTH - enemy->radius;
        enemy->velocity.x = -enemy->velocity.x;
    }
    if (enemy->position.y < enemy->radius) {
        enemy->position.y = enemy->radius;
        enemy->velocity.y = -enemy->velocity.y;
    }
    if (enemy->position.y > SCREEN_HEIGHT - enemy->radius) {
        enemy->position.y = SCREEN_HEIGHT - enemy->radius;
        enemy->velocity.y = -enemy->velocity.y;
    }
}

bool enemy_check_collision_with_player(const Enemy* enemy, Vector2 player_position, float player_radius) {
    if (!enemy) return false;
    
    float distance = sqrtf((player_position.x - enemy->position.x) * (player_position.x - enemy->position.x) +
                           (player_position.y - enemy->position.y) * (player_position.y - enemy->position.y));
    return distance < player_radius + enemy->radius;
}

bool enemy_check_wall_collision(const Enemy* enemy, Vector2 new_position, const struct Map* current_map) {
    if (!enemy || !current_map) return false;
    
    Rectangle enemy_rect = {
        new_position.x - enemy->radius,
        new_position.y - enemy->radius,
        enemy->radius * 2,
        enemy->radius * 2
    };
    
    int wall_count;
    const Wall* walls = map_get_walls(current_map, &wall_count);
    for (int i = 0; i < wall_count; i++) {
        if (CheckCollisionRecs(enemy_rect, walls[i].rect)) {
            return true;
        }
    }
    return false;
}
