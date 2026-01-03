#include "../include/map.h"
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#define OBSTACLE_SPEED 2.0f
#define OBSTACLE_DIRECTION_CHANGE_FRAMES 120

bool map_check_circle_rect_collision(Vector2 circle_pos, float radius, Rectangle rect) {
    float closest_x = fmaxf(rect.x, fminf(circle_pos.x, rect.x + rect.width));
    float closest_y = fmaxf(rect.y, fminf(circle_pos.y, rect.y + rect.height));
    
    float distance_x = circle_pos.x - closest_x;
    float distance_y = circle_pos.y - closest_y;
    
    return (distance_x * distance_x + distance_y * distance_y) < (radius * radius);
}

bool map_is_valid_spawn_position(Vector2 position, float radius, const Map* map) {
    if (position.x < radius || position.x > SCREEN_WIDTH - radius ||
        position.y < radius || position.y > SCREEN_HEIGHT - radius) {
        return false;
    }
    
    for (int i = 0; i < map->wall_count; i++) {
        if (map_check_circle_rect_collision(position, radius, map->walls[i].rect)) {
            return false;
        }
    }
    
    return true;
}

Vector2 map_find_valid_spawn_position(Vector2 desired_pos, float radius, const Map* map) {
    if (map_is_valid_spawn_position(desired_pos, radius, map)) {
        return desired_pos;
    }
    
    float search_radius = radius * 2;
    int max_attempts = 50;
    
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        for (int angle_step = 0; angle_step < 8; angle_step++) {
            float angle = (angle_step * 45.0f) * DEG2RAD;
            Vector2 test_pos = {
                desired_pos.x + cosf(angle) * search_radius,
                desired_pos.y + sinf(angle) * search_radius
            };
            
            if (map_is_valid_spawn_position(test_pos, radius, map)) {
                return test_pos;
            }
        }
        
        search_radius += radius;
    }
    
    Vector2 safe_pos = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    return safe_pos;
}

Map* map_create(int map_id) {
    Map* map = (Map*)malloc(sizeof(Map));
    if (!map) return NULL;
    map_init(map, map_id);
    return map;
}

void map_destroy(Map* map) {
    if (map) free(map);
}

void map_init(Map* map, int map_id) {
    map->map_id = map_id;
    map->wall_count = 0;
    map->exit_count = 0;
    map->entrance_count = 0;
    map->coin_count = 0;
    map->obstacle_count = 0;
    
    switch(map_id) {
        case 0: map->bg_color = (Color){240, 240, 255, 255}; break;
        case 1: map->bg_color = (Color){255, 240, 240, 255}; break;
        case 2: map->bg_color = (Color){240, 255, 240, 255}; break;
        case 3: map->bg_color = (Color){255, 255, 240, 255}; break;
        default: map->bg_color = RAYWHITE; break;
    }
    
    if (map_id == 0) {
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 50, 150, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){250, 50, 150, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 50, 20, 150}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 250, 20, 150}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){200, 200, 100, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){200, 200, 20, 100}};
        
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH - EXIT_WIDTH - 20, SCREEN_HEIGHT/2 - EXIT_HEIGHT/2, EXIT_WIDTH, EXIT_HEIGHT}, 1, 0};
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH/2 - EXIT_WIDTH/2, SCREEN_HEIGHT - EXIT_HEIGHT - 20, EXIT_WIDTH, EXIT_HEIGHT}, 2, 0};
        
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}};
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){50, SCREEN_HEIGHT/2}};
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, 50}};
        
        map->coins[map->coin_count++] = (Coin){(Vector2){150, 150}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){350, 200}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){150, 350}, false};
        
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){300, 250}, (Vector2){OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 0, RED};
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){200, 300}, (Vector2){-OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 60, RED};
        
        for (int i = 0; i < map->entrance_count; i++) {
            Vector2 valid_pos = map_find_valid_spawn_position(map->entrances[i].position, PLAYER_RADIUS, map);
            if (valid_pos.x != map->entrances[i].position.x || valid_pos.y != map->entrances[i].position.y) {
                printf("Map %d: Adjusted entrance %d\n", map_id, i);
            }
            map->entrances[i].position = valid_pos;
        }
        
        for (int i = 0; i < map->obstacle_count; i++) {
            Vector2 valid_pos = map_find_valid_spawn_position(map->obstacles[i].position, OBSTACLE_RADIUS, map);
            map->obstacles[i].position = valid_pos;
        }
    }
    else if (map_id == 1) {
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 50, 150, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){650, 50, 150, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 50, 20, 150}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 250, 20, 150}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){600, 200, 100, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){600, 200, 20, 100}};
        
        map->exits[map->exit_count++] = (Exit){(Rectangle){20, SCREEN_HEIGHT/2 - EXIT_HEIGHT/2, EXIT_WIDTH, EXIT_HEIGHT}, 0, 1};
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH/2 - EXIT_WIDTH/2, SCREEN_HEIGHT - EXIT_HEIGHT - 20, EXIT_WIDTH, EXIT_HEIGHT}, 3, 0};
        
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}};
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH - 50, SCREEN_HEIGHT/2}};
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, 50}};
        
        map->coins[map->coin_count++] = (Coin){(Vector2){550, 150}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){750, 200}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){550, 350}, false};
        
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){600, 250}, (Vector2){OBSTACLE_SPEED, -OBSTACLE_SPEED}, OBSTACLE_RADIUS, 30, RED};
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){700, 300}, (Vector2){-OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 90, RED};
        
        for (int i = 0; i < map->entrance_count; i++) {
            map->entrances[i].position = map_find_valid_spawn_position(map->entrances[i].position, PLAYER_RADIUS, map);
        }
        for (int i = 0; i < map->obstacle_count; i++) {
            map->obstacles[i].position = map_find_valid_spawn_position(map->obstacles[i].position, OBSTACLE_RADIUS, map);
        }
    }
    else if (map_id == 2) {
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 400, 150, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){250, 400, 150, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 400, 20, 150}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){50, 600, 20, 150}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){200, 550, 100, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){200, 550, 20, 100}};
        
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH/2 - EXIT_WIDTH/2, 20, EXIT_WIDTH, EXIT_HEIGHT}, 0, 2};
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH - EXIT_WIDTH - 20, SCREEN_HEIGHT/2 - EXIT_HEIGHT/2, EXIT_WIDTH, EXIT_HEIGHT}, 3, 1};
        
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}};
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT - 50}};
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){50, SCREEN_HEIGHT/2}};
        
        map->coins[map->coin_count++] = (Coin){(Vector2){150, 500}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){350, 450}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){150, 350}, false};
        
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){300, 500}, (Vector2){OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 45, RED};
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){200, 450}, (Vector2){-OBSTACLE_SPEED, OBSTACLE_SPEED}, OBSTACLE_RADIUS, 120, RED};
        
        for (int i = 0; i < map->entrance_count; i++) {
            map->entrances[i].position = map_find_valid_spawn_position(map->entrances[i].position, PLAYER_RADIUS, map);
        }
        for (int i = 0; i < map->obstacle_count; i++) {
            map->obstacles[i].position = map_find_valid_spawn_position(map->obstacles[i].position, OBSTACLE_RADIUS, map);
        }
    }
    else if (map_id == 3) {
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 400, 150, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){650, 400, 150, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 400, 20, 150}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){450, 600, 20, 150}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){600, 550, 100, 20}};
        map->walls[map->wall_count++] = (Wall){(Rectangle){600, 550, 20, 100}};
        
        map->exits[map->exit_count++] = (Exit){(Rectangle){SCREEN_WIDTH/2 - EXIT_WIDTH/2, 20, EXIT_WIDTH, EXIT_HEIGHT}, 1, 2};
        map->exits[map->exit_count++] = (Exit){(Rectangle){20, SCREEN_HEIGHT/2 - EXIT_HEIGHT/2, EXIT_WIDTH, EXIT_HEIGHT}, 2, 2};
        
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}};
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT - 50}};
        map->entrances[map->entrance_count++] = (Entrance){(Vector2){SCREEN_WIDTH - 50, SCREEN_HEIGHT/2}};
        
        map->coins[map->coin_count++] = (Coin){(Vector2){550, 500}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){750, 450}, false};
        map->coins[map->coin_count++] = (Coin){(Vector2){550, 350}, false};
        
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){600, 500}, (Vector2){OBSTACLE_SPEED, -OBSTACLE_SPEED}, OBSTACLE_RADIUS, 75, RED};
        map->obstacles[map->obstacle_count++] = (Obstacle){(Vector2){700, 450}, (Vector2){-OBSTACLE_SPEED, -OBSTACLE_SPEED}, OBSTACLE_RADIUS, 15, RED};
        
        for (int i = 0; i < map->entrance_count; i++) {
            map->entrances[i].position = map_find_valid_spawn_position(map->entrances[i].position, PLAYER_RADIUS, map);
        }
        for (int i = 0; i < map->obstacle_count; i++) {
            map->obstacles[i].position = map_find_valid_spawn_position(map->obstacles[i].position, OBSTACLE_RADIUS, map);
        }
    }
}

const Wall* map_get_walls(const Map* map, int* count) {
    if (count) *count = map->wall_count;
    return map->walls;
}

const Exit* map_get_exits(const Map* map, int* count) {
    if (count) *count = map->exit_count;
    return map->exits;
}

const Entrance* map_get_entrances(const Map* map, int* count) {
    if (count) *count = map->entrance_count;
    return map->entrances;
}

const Coin* map_get_coins(const Map* map, int* count) {
    if (count) *count = map->coin_count;
    return map->coins;
}

const Obstacle* map_get_obstacles(const Map* map, int* count) {
    if (count) *count = map->obstacle_count;
    return map->obstacles;
}

Color map_get_background_color(const Map* map) {
    return map->bg_color;
}

int map_get_id(const Map* map) {
    return map->map_id;
}

Coin* map_get_coin_mutable(Map* map, int index) {
    if (!map || index < 0 || index >= map->coin_count) return NULL;
    return &map->coins[index];
}

Obstacle* map_get_obstacle_mutable(Map* map, int index) {
    if (!map || index < 0 || index >= map->obstacle_count) return NULL;
    return &map->obstacles[index];
}

void map_update_obstacle(Map* map, int index, Vector2 new_position, Vector2 new_velocity, int new_timer) {
    if (!map || index < 0 || index >= map->obstacle_count) return;
    map->obstacles[index].position = new_position;
    map->obstacles[index].velocity = new_velocity;
    map->obstacles[index].direction_change_timer = new_timer;
}
