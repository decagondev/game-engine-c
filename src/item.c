#include "../include/item.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Item constants per type (Open/Closed Principle - easy to extend)
#define COIN_RADIUS 15.0f
#define POWERUP_RADIUS 20.0f
#define HEALTH_PACK_RADIUS 18.0f
#define KEY_RADIUS 12.0f

// Item structure (Single Responsibility - item state)
struct Item {
    ItemType type;
    Vector2 position;
    bool collected;
};

// Item creation
Item* item_create(ItemType type, Vector2 position) {
    Item* item = (Item*)malloc(sizeof(Item));
    if (!item) return NULL;
    
    item->type = type;
    item->position = position;
    item->collected = false;
    
    return item;
}

// Item destruction
void item_destroy(Item* item) {
    if (item) free(item);
}

// Item state queries
ItemType item_get_type(const Item* item) {
    if (!item) return ITEM_TYPE_COIN;
    return item->type;
}

Vector2 item_get_position(const Item* item) {
    if (!item) return (Vector2){0, 0};
    return item->position;
}

bool item_is_collected(const Item* item) {
    if (!item) return true;
    return item->collected;
}

// Item state modifications
void item_set_position(Item* item, Vector2 position) {
    if (!item) return;
    item->position = position;
}

void item_set_collected(Item* item, bool collected) {
    if (!item) return;
    item->collected = collected;
}

void item_collect(Item* item) {
    if (!item || item->collected) return;
    item->collected = true;
    item_on_collect(item);
}

// Item collision detection
bool item_check_collision_with_player(const Item* item, Vector2 player_position, float player_radius) {
    if (!item || item->collected) return false;
    
    float item_radius = item_get_radius(item->type);
    float distance = sqrtf((player_position.x - item->position.x) * (player_position.x - item->position.x) +
                           (player_position.y - item->position.y) * (player_position.y - item->position.y));
    return distance < player_radius + item_radius;
}

// Item behavior (Open/Closed Principle - extensible)
void item_on_collect(Item* item) {
    if (!item) return;
    
    // Default behavior - can be extended for different item types
    // For example, coins might play a sound, powerups might grant abilities, etc.
    switch (item->type) {
        case ITEM_TYPE_COIN:
            // Coin collection behavior is handled by game logic
            break;
        case ITEM_TYPE_POWERUP:
            // Future: Grant powerup ability
            break;
        case ITEM_TYPE_HEALTH_PACK:
            // Future: Restore health
            break;
        case ITEM_TYPE_KEY:
            // Future: Unlock doors
            break;
        default:
            break;
    }
}

// Item constants per type
float item_get_radius(ItemType type) {
    switch (type) {
        case ITEM_TYPE_COIN:
            return COIN_RADIUS;
        case ITEM_TYPE_POWERUP:
            return POWERUP_RADIUS;
        case ITEM_TYPE_HEALTH_PACK:
            return HEALTH_PACK_RADIUS;
        case ITEM_TYPE_KEY:
            return KEY_RADIUS;
        default:
            return COIN_RADIUS;
    }
}

Color item_get_color(ItemType type) {
    switch (type) {
        case ITEM_TYPE_COIN:
            return GOLD;
        case ITEM_TYPE_POWERUP:
            return PURPLE;
        case ITEM_TYPE_HEALTH_PACK:
            return GREEN;
        case ITEM_TYPE_KEY:
            return YELLOW;
        default:
            return GOLD;
    }
}

