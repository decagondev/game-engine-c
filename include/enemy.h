#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <stdbool.h>

// Forward declarations
struct Map;

// Enemy constants
#define ENEMY_SPEED 2.0f
#define ENEMY_RADIUS 20.0f
#define ENEMY_DIRECTION_CHANGE_FRAMES 120

// Enemy structure (opaque - Single Responsibility)
typedef struct Enemy Enemy;

// Enemy creation/destruction (Interface Segregation)
Enemy* enemy_create(Vector2 position, Vector2 velocity, Color color);
void enemy_destroy(Enemy* enemy);

// Enemy state queries (Interface Segregation - separate query methods)
Vector2 enemy_get_position(const Enemy* enemy);
Vector2 enemy_get_velocity(const Enemy* enemy);
float enemy_get_radius(const Enemy* enemy);
Color enemy_get_color(const Enemy* enemy);
int enemy_get_direction_timer(const Enemy* enemy);

// Enemy state modifications (Interface Segregation - separate modification methods)
void enemy_set_position(Enemy* enemy, Vector2 position);
void enemy_set_velocity(Enemy* enemy, Vector2 velocity);
void enemy_set_direction_timer(Enemy* enemy, int timer);

// Enemy update logic (Single Responsibility - enemy behavior)
void enemy_update(Enemy* enemy, const struct Map* current_map);

// Enemy collision detection (Single Responsibility - collision logic)
bool enemy_check_collision_with_player(const Enemy* enemy, Vector2 player_position, float player_radius);
bool enemy_check_wall_collision(const Enemy* enemy, Vector2 new_position, const struct Map* current_map);

#endif // ENEMY_H

