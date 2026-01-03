#include "../include/raycaster.h"
#include "../include/map.h"
#include <math.h>
#include <float.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

RaycastResult raycaster_cast_ray(Vector2 start_pos, float angle, const Map* map) {
    RaycastResult result = {0};
    result.hit = false;
    result.distance = RAYCASTER_MAX_DISTANCE;
    result.color = DARKGRAY;
    
    if (!map) return result;
    
    // Direction vector
    float dx = cosf(angle);
    float dy = sinf(angle);
    
    // Step size for DDA algorithm
    float delta_dist_x = (dx == 0) ? FLT_MAX : fabsf(1.0f / dx);
    float delta_dist_y = (dy == 0) ? FLT_MAX : fabsf(1.0f / dy);
    
    // Current grid position
    int map_x = (int)start_pos.x;
    int map_y = (int)start_pos.y;
    
    // Step direction
    int step_x, step_y;
    float side_dist_x, side_dist_y;
    
    if (dx < 0) {
        step_x = -1;
        side_dist_x = (start_pos.x - map_x) * delta_dist_x;
    } else {
        step_x = 1;
        side_dist_x = (map_x + 1.0f - start_pos.x) * delta_dist_x;
    }
    
    if (dy < 0) {
        step_y = -1;
        side_dist_y = (start_pos.y - map_y) * delta_dist_y;
    } else {
        step_y = 1;
        side_dist_y = (map_y + 1.0f - start_pos.y) * delta_dist_y;
    }
    
    // DDA algorithm
    bool hit = false;
    int side = 0; // 0 = x-side, 1 = y-side
    int wall_index = -1;
    
    // Check walls from map
    int wall_count;
    const Wall* walls = map_get_walls(map, &wall_count);
    
    float min_dist = RAYCASTER_MAX_DISTANCE;
    Vector2 hit_point = {0, 0};
    Color hit_color = DARKGRAY;
    
    // Cast ray and check against all walls
    for (int i = 0; i < wall_count; i++) {
        Rectangle wall = walls[i].rect;
        
        // Check if ray intersects with this wall rectangle
        // Using line-rectangle intersection
        Vector2 ray_end = {
            start_pos.x + dx * RAYCASTER_MAX_DISTANCE,
            start_pos.y + dy * RAYCASTER_MAX_DISTANCE
        };
        
        // Line-rectangle intersection
        float t_min = 0.0f;
        float t_max = 1.0f;
        
        // Check x-axis
        if (dx != 0) {
            float t1 = (wall.x - start_pos.x) / dx;
            float t2 = (wall.x + wall.width - start_pos.x) / dx;
            float t_near = fminf(t1, t2);
            float t_far = fmaxf(t1, t2);
            
            if (t_near > t_max || t_far < t_min) continue;
            t_min = fmaxf(t_min, t_near);
            t_max = fminf(t_max, t_far);
        } else {
            if (start_pos.x < wall.x || start_pos.x > wall.x + wall.width) continue;
        }
        
        // Check y-axis
        if (dy != 0) {
            float t1 = (wall.y - start_pos.y) / dy;
            float t2 = (wall.y + wall.height - start_pos.y) / dy;
            float t_near = fminf(t1, t2);
            float t_far = fmaxf(t1, t2);
            
            if (t_near > t_max || t_far < t_min) continue;
            t_min = fmaxf(t_min, t_near);
            t_max = fminf(t_max, t_far);
        } else {
            if (start_pos.y < wall.y || start_pos.y > wall.y + wall.height) continue;
        }
        
        if (t_min <= t_max && t_min >= 0 && t_min < 1.0f) {
            // Calculate actual distance along the ray
            Vector2 intersection = {
                start_pos.x + dx * t_min * RAYCASTER_MAX_DISTANCE,
                start_pos.y + dy * t_min * RAYCASTER_MAX_DISTANCE
            };
            float dx_intersect = intersection.x - start_pos.x;
            float dy_intersect = intersection.y - start_pos.y;
            float dist = sqrtf(dx_intersect * dx_intersect + dy_intersect * dy_intersect);
            
            if (dist < min_dist && dist > 0.1f) {  // Avoid self-intersection
                min_dist = dist;
                hit_point = intersection;
                hit_color = DARKGRAY;
                wall_index = i;
                hit = true;
                
                // Determine which side of the wall was hit
                // Check which edge of the rectangle is closest to the intersection
                float dist_to_left = fabsf(intersection.x - wall.x);
                float dist_to_right = fabsf(intersection.x - (wall.x + wall.width));
                float dist_to_top = fabsf(intersection.y - wall.y);
                float dist_to_bottom = fabsf(intersection.y - (wall.y + wall.height));
                
                float min_edge_dist = fminf(fminf(dist_to_left, dist_to_right), 
                                            fminf(dist_to_top, dist_to_bottom));
                
                if (min_edge_dist == dist_to_left) {
                    result.side = WALL_SIDE_WEST;
                } else if (min_edge_dist == dist_to_right) {
                    result.side = WALL_SIDE_EAST;
                } else if (min_edge_dist == dist_to_top) {
                    result.side = WALL_SIDE_NORTH;
                } else {
                    result.side = WALL_SIDE_SOUTH;
                }
            }
        }
    }
    
    if (hit) {
        result.hit = true;
        result.distance = min_dist;
        result.hit_point = hit_point;
        result.wall_index = wall_index;
        
        // Calculate perpendicular distance to avoid fisheye effect
        result.perp_distance = min_dist * cosf(angle - atan2f(dy, dx));
        if (result.perp_distance < 0.1f) result.perp_distance = 0.1f;
        
        result.color = raycaster_get_shaded_color(hit_color, result.perp_distance, result.side);
        
        // Calculate wall height based on perpendicular distance (perspective)
        result.wall_height = (RAYCASTER_WALL_HEIGHT / result.perp_distance) * 200.0f;
        if (result.wall_height > SCREEN_HEIGHT) result.wall_height = SCREEN_HEIGHT;
    } else {
        result.side = WALL_SIDE_UNKNOWN;
        result.perp_distance = RAYCASTER_MAX_DISTANCE;
    }
    
    return result;
}

Color raycaster_get_shaded_color(Color base_color, float distance, WallSide side) {
    // Base distance-based shading (darker with distance)
    float distance_shade = 1.0f / (1.0f + distance * 0.008f);
    if (distance_shade > 1.0f) distance_shade = 1.0f;
    if (distance_shade < 0.15f) distance_shade = 0.15f;  // Minimum brightness
    
    // Side-based shading (simulate light from above)
    float side_shade = 1.0f;
    switch (side) {
        case WALL_SIDE_NORTH:
            side_shade = 0.9f;  // Slightly darker (facing away from typical light)
            break;
        case WALL_SIDE_SOUTH:
            side_shade = 1.0f;  // Brightest (facing light)
            break;
        case WALL_SIDE_EAST:
            side_shade = 0.85f;  // Medium dark
            break;
        case WALL_SIDE_WEST:
            side_shade = 0.85f;  // Medium dark
            break;
        default:
            side_shade = 1.0f;
            break;
    }
    
    float total_shade = distance_shade * side_shade;
    
    Color shaded = {
        (unsigned char)(base_color.r * total_shade),
        (unsigned char)(base_color.g * total_shade),
        (unsigned char)(base_color.b * total_shade),
        base_color.a
    };
    
    return shaded;
}

Vector2 raycaster_world_to_grid(Vector2 pos, float cell_size) {
    return (Vector2){
        floorf(pos.x / cell_size),
        floorf(pos.y / cell_size)
    };
}

int raycaster_find_visible_enemies(Vector2 start_pos, float angle, const Map* map, float max_distance,
                                   Vector2* enemy_positions, int enemy_count,
                                   float* distances, float* sprite_angles) {
    if (!map || !enemy_positions || !distances || !sprite_angles) return 0;
    
    int visible_count = 0;
    float fov_half = RAYCASTER_FOV * DEG2RAD / 2.0f;
    
    for (int i = 0; i < enemy_count; i++) {
        Vector2 enemy_pos = enemy_positions[i];
        Vector2 to_enemy = {
            enemy_pos.x - start_pos.x,
            enemy_pos.y - start_pos.y
        };
        
        float dist = sqrtf(to_enemy.x * to_enemy.x + to_enemy.y * to_enemy.y);
        
        if (dist > max_distance || dist < 0.1f) continue;
        
        // Check if enemy is within FOV
        float enemy_angle = atan2f(to_enemy.y, to_enemy.x);
        float angle_diff = enemy_angle - angle;
        
        // Normalize angle difference to [-PI, PI]
        while (angle_diff > PI) angle_diff -= 2.0f * PI;
        while (angle_diff < -PI) angle_diff += 2.0f * PI;
        
        if (fabsf(angle_diff) <= fov_half) {
            // Check if there's a wall blocking the view
            RaycastResult result = raycaster_cast_ray(start_pos, enemy_angle, map);
            if (!result.hit || result.distance > dist) {
                distances[visible_count] = dist;
                sprite_angles[visible_count] = angle_diff;
                visible_count++;
            }
        }
    }
    
    return visible_count;
}

