#ifndef ITEM_H
#define ITEM_H

#include "raylib.h"
#include <stdbool.h>

// Item types (Open/Closed Principle - easy to extend)
typedef enum {
    ITEM_TYPE_COIN,
    ITEM_TYPE_POWERUP,      // For future expansion
    ITEM_TYPE_HEALTH_PACK,  // For future expansion
    ITEM_TYPE_KEY,          // For future expansion
    ITEM_TYPE_COUNT         // Count of item types
} ItemType;

// Item structure (opaque - Single Responsibility)
typedef struct Item Item;

// Item creation/destruction (Interface Segregation)
Item* item_create(ItemType type, Vector2 position);
void item_destroy(Item* item);

// Item state queries (Interface Segregation - separate query methods)
ItemType item_get_type(const Item* item);
Vector2 item_get_position(const Item* item);
bool item_is_collected(const Item* item);

// Item state modifications (Interface Segregation - separate modification methods)
void item_set_position(Item* item, Vector2 position);
void item_set_collected(Item* item, bool collected);
void item_collect(Item* item); // Marks item as collected

// Item collision detection (Single Responsibility - collision logic)
bool item_check_collision_with_player(const Item* item, Vector2 player_position, float player_radius);

// Item behavior (Open/Closed Principle - extensible for different item types)
void item_on_collect(Item* item); // Called when item is collected (can be extended for different behaviors)

// Item constants (per type)
float item_get_radius(ItemType type);
Color item_get_color(ItemType type);

#endif // ITEM_H

