#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

// Forward declarations
struct Map;

// Player constants
#define PLAYER_SPEED 5.0f
#define PLAYER_RADIUS 25.0f
#define MAX_HEALTH 100.0f
#define DAMAGE_PER_HIT 10.0f
#define INVINCIBILITY_FRAMES 60

// Player structure (opaque - Single Responsibility)
typedef struct Player Player;

// Player creation/destruction (Interface Segregation)
Player* player_create(void);
void player_destroy(Player* player);

// Player initialization
void player_init(Player* player, Vector2 start_position);

// Player state queries (Interface Segregation - separate query methods)
Vector2 player_get_position(const Player* player);
float player_get_health(const Player* player);
float player_get_max_health(const Player* player);
bool player_is_invincible(const Player* player);
int player_get_invincibility_timer(const Player* player);
bool player_is_alive(const Player* player);

// Player state modifications (Interface Segregation - separate modification methods)
void player_set_position(Player* player, Vector2 position);
void player_set_health(Player* player, float health);
void player_reset(Player* player, Vector2 start_position);

// Player movement (Single Responsibility - movement logic)
void player_update_movement(Player* player, const struct Map* current_map);
void player_handle_input(Player* player);

// Player collision detection (Single Responsibility - collision logic)
bool player_check_wall_collision(const Player* player, Vector2 new_position, const struct Map* current_map);
bool player_check_exit_collision(const Player* player, const struct Map* current_map, int* target_map_id, int* target_entrance_id);
void player_apply_damage(Player* player, float damage);
void player_update(Player* player); // Update invincibility timer, etc.

#endif // PLAYER_H

