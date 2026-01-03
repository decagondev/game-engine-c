#ifndef ITEM_H
#define ITEM_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    ITEM_TYPE_COIN,
    ITEM_TYPE_POWERUP,
    ITEM_TYPE_HEALTH_PACK,
    ITEM_TYPE_KEY,
    ITEM_TYPE_COUNT
} ItemType;

typedef struct Item Item;

/**
 * Create a new item instance.
 * @param type Type of item
 * @param position Position of the item
 * @return Pointer to created item, or NULL on failure
 */
Item* item_create(ItemType type, Vector2 position);

/**
 * Destroy an item instance.
 * @param item The item to destroy
 */
void item_destroy(Item* item);

/**
 * Get the item's type.
 * @param item The item
 * @return Item type
 */
ItemType item_get_type(const Item* item);

/**
 * Get the item's position.
 * @param item The item
 * @return Position
 */
Vector2 item_get_position(const Item* item);

/**
 * Check if the item has been collected.
 * @param item The item
 * @return true if collected, false otherwise
 */
bool item_is_collected(const Item* item);

/**
 * Set the item's position.
 * @param item The item
 * @param position New position
 */
void item_set_position(Item* item, Vector2 position);

/**
 * Set the item's collected state.
 * @param item The item
 * @param collected Collected state
 */
void item_set_collected(Item* item, bool collected);

/**
 * Mark the item as collected.
 * @param item The item
 */
void item_collect(Item* item);

/**
 * Check if item is colliding with the player.
 * @param item The item
 * @param player_position Player's position
 * @param player_radius Player's radius
 * @return true if collision detected, false otherwise
 */
bool item_check_collision_with_player(const Item* item, Vector2 player_position, float player_radius);

/**
 * Called when item is collected (can be extended for different behaviors).
 * @param item The item
 */
void item_on_collect(Item* item);

/**
 * Get the radius for a specific item type.
 * @param type Item type
 * @return Radius
 */
float item_get_radius(ItemType type);

/**
 * Get the color for a specific item type.
 * @param type Item type
 * @return Color
 */
Color item_get_color(ItemType type);

#endif
