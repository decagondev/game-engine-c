#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <stdbool.h>

typedef struct Map Map;
typedef struct Wall Wall;
typedef struct Exit Exit;
typedef struct Entrance Entrance;
typedef struct Coin Coin;
typedef struct Obstacle Obstacle;

#define MAX_WALLS 20
#define MAX_EXITS 4
#define MAX_ENTRANCES 4
#define MAX_COINS 10
#define MAX_OBSTACLES 5
#define NUM_MAPS 4

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define EXIT_WIDTH 60.0f
#define EXIT_HEIGHT 60.0f
#define PLAYER_RADIUS 25.0f
#define OBSTACLE_RADIUS 20.0f

struct Wall {
    Rectangle rect;
};

struct Exit {
    Rectangle rect;
    int target_map_id;
    int target_entrance_id;
};

struct Entrance {
    Vector2 position;
};

struct Coin {
    Vector2 position;
    bool collected;
};

struct Obstacle {
    Vector2 position;
    Vector2 velocity;
    float radius;
    int direction_change_timer;
    Color color;
};

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

/**
 * Create a new map instance.
 * @param map_id The ID of the map to create
 * @return Pointer to created map, or NULL on failure
 */
Map* map_create(int map_id);

/**
 * Destroy a map instance.
 * @param map The map to destroy
 */
void map_destroy(Map* map);

/**
 * Initialize a map with the specified ID.
 * @param map The map to initialize
 * @param map_id The ID of the map
 */
void map_init(Map* map, int map_id);

/**
 * Check if a circle collides with a rectangle.
 * @param circle_pos Center position of the circle
 * @param radius Radius of the circle
 * @param rect Rectangle to check collision with
 * @return true if collision detected, false otherwise
 */
bool map_check_circle_rect_collision(Vector2 circle_pos, float radius, Rectangle rect);

/**
 * Check if a position is valid for spawning (not colliding with walls).
 * @param position Position to check
 * @param radius Radius of the entity to spawn
 * @param map The map to check against
 * @return true if position is valid, false otherwise
 */
bool map_is_valid_spawn_position(Vector2 position, float radius, const Map* map);

/**
 * Find a valid spawn position near the desired position.
 * @param desired_pos Desired spawn position
 * @param radius Radius of the entity to spawn
 * @param map The map to search in
 * @return Valid spawn position
 */
Vector2 map_find_valid_spawn_position(Vector2 desired_pos, float radius, const Map* map);

/**
 * Get walls from a map.
 * @param map The map to query
 * @param count Output parameter for wall count
 * @return Pointer to walls array
 */
const Wall* map_get_walls(const Map* map, int* count);

/**
 * Get exits from a map.
 * @param map The map to query
 * @param count Output parameter for exit count
 * @return Pointer to exits array
 */
const Exit* map_get_exits(const Map* map, int* count);

/**
 * Get entrances from a map.
 * @param map The map to query
 * @param count Output parameter for entrance count
 * @return Pointer to entrances array
 */
const Entrance* map_get_entrances(const Map* map, int* count);

/**
 * Get coins from a map.
 * @param map The map to query
 * @param count Output parameter for coin count
 * @return Pointer to coins array
 */
const Coin* map_get_coins(const Map* map, int* count);

/**
 * Get obstacles from a map.
 * @param map The map to query
 * @param count Output parameter for obstacle count
 * @return Pointer to obstacles array
 */
const Obstacle* map_get_obstacles(const Map* map, int* count);

/**
 * Get the background color of a map.
 * @param map The map to query
 * @return Background color
 */
Color map_get_background_color(const Map* map);

/**
 * Get the ID of a map.
 * @param map The map to query
 * @return Map ID
 */
int map_get_id(const Map* map);

/**
 * Get a mutable reference to a coin in a map.
 * @param map The map containing the coin
 * @param index Index of the coin
 * @return Pointer to coin, or NULL if invalid index
 */
Coin* map_get_coin_mutable(Map* map, int index);

/**
 * Get a mutable reference to an obstacle in a map.
 * @param map The map containing the obstacle
 * @param index Index of the obstacle
 * @return Pointer to obstacle, or NULL if invalid index
 */
Obstacle* map_get_obstacle_mutable(Map* map, int index);

/**
 * Update an obstacle's state in a map.
 * @param map The map containing the obstacle
 * @param index Index of the obstacle
 * @param new_position New position of the obstacle
 * @param new_velocity New velocity of the obstacle
 * @param new_timer New direction change timer value
 */
void map_update_obstacle(Map* map, int index, Vector2 new_position, Vector2 new_velocity, int new_timer);

#endif
