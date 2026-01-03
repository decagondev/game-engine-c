#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <stdbool.h>

// Forward declarations
typedef struct Map Map;
typedef struct Wall Wall;
typedef struct Exit Exit;
typedef struct Entrance Entrance;
typedef struct Coin Coin;
typedef struct Obstacle Obstacle;

// Map constants
#define MAX_WALLS 20
#define MAX_EXITS 4
#define MAX_ENTRANCES 4
#define MAX_COINS 10
#define MAX_OBSTACLES 5
#define NUM_MAPS 4

// Game constants (used by map system)
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define EXIT_WIDTH 60.0f
#define EXIT_HEIGHT 60.0f
#define PLAYER_RADIUS 25.0f
#define OBSTACLE_RADIUS 20.0f

// Wall structure
struct Wall {
    Rectangle rect;
};

// Exit structure
struct Exit {
    Rectangle rect;
    int target_map_id;
    int target_entrance_id;
};

// Entrance structure
struct Entrance {
    Vector2 position;
};

// Coin structure
struct Coin {
    Vector2 position;
    bool collected;
};

// Obstacle structure
struct Obstacle {
    Vector2 position;
    Vector2 velocity;
    float radius;
    int direction_change_timer;
    Color color;
};

// Map structure (opaque - Single Responsibility)
struct Map {
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
};

// Map management interface (Interface Segregation)
Map* map_create(int map_id);
void map_destroy(Map* map);

// Map initialization
void map_init(Map* map, int map_id);

// Collision detection (Single Responsibility)
bool map_check_circle_rect_collision(Vector2 circle_pos, float radius, Rectangle rect);
bool map_is_valid_spawn_position(Vector2 position, float radius, const Map* map);
Vector2 map_find_valid_spawn_position(Vector2 desired_pos, float radius, const Map* map);

// Map queries (Interface Segregation - separate query methods)
const Wall* map_get_walls(const Map* map, int* count);
const Exit* map_get_exits(const Map* map, int* count);
const Entrance* map_get_entrances(const Map* map, int* count);
const Coin* map_get_coins(const Map* map, int* count);
const Obstacle* map_get_obstacles(const Map* map, int* count);
Color map_get_background_color(const Map* map);
int map_get_id(const Map* map);

// Map modifications (Interface Segregation - separate modification methods)
Coin* map_get_coin_mutable(Map* map, int index);
Obstacle* map_get_obstacle_mutable(Map* map, int index);
void map_update_obstacle(Map* map, int index, Vector2 new_position, Vector2 new_velocity, int new_timer);

#endif // MAP_H

