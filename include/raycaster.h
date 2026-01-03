#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "raylib.h"
#include "map.h"
#include <stdbool.h>

#define RAYCASTER_FOV 60.0f  // Field of view in degrees
#define RAYCASTER_MAX_DISTANCE 1000.0f
#define RAYCASTER_WALL_HEIGHT 200.0f

typedef enum {
    WALL_SIDE_NORTH,
    WALL_SIDE_SOUTH,
    WALL_SIDE_EAST,
    WALL_SIDE_WEST,
    WALL_SIDE_UNKNOWN
} WallSide;

typedef struct {
    float distance;
    float wall_height;
    Color color;
    bool hit;
    Vector2 hit_point;
    int wall_index;
    WallSide side;  // Which side of the wall was hit
    float perp_distance;  // Perpendicular distance (for fisheye correction)
} RaycastResult;

/**
 * Cast a ray from a position in a direction and find the first wall hit.
 * @param start_pos Starting position
 * @param angle Angle in radians (0 = right, PI/2 = down, PI = left, 3*PI/2 = up)
 * @param map The map to cast against
 * @return Raycast result with distance and hit information
 */
RaycastResult raycaster_cast_ray(Vector2 start_pos, float angle, const Map* map);

/**
 * Get the color for a wall based on distance and side (for shading).
 * @param base_color Base color of the wall
 * @param distance Distance to the wall
 * @param side Which side of the wall was hit
 * @return Shaded color
 */
Color raycaster_get_shaded_color(Color base_color, float distance, WallSide side);

/**
 * Cast a ray to find enemies/sprites in the view.
 * @param start_pos Starting position
 * @param angle Angle in radians
 * @param map The map to check
 * @param max_distance Maximum distance to check
 * @param enemy_positions Array of enemy positions
 * @param enemy_count Number of enemies
 * @param distances Output array for distances
 * @param sprite_angles Output array for sprite angles relative to player
 * @return Number of visible enemies
 */
int raycaster_find_visible_enemies(Vector2 start_pos, float angle, const Map* map, float max_distance,
                                   Vector2* enemy_positions, int enemy_count,
                                   float* distances, float* sprite_angles);

/**
 * Convert 2D map coordinates to a grid cell.
 * @param pos Position in world coordinates
 * @param cell_size Size of each grid cell
 * @return Grid cell coordinates
 */
Vector2 raycaster_world_to_grid(Vector2 pos, float cell_size);

#endif

