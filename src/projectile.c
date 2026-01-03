#include "../include/projectile.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct Projectile {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float damage;
    int lifetime;
    int max_lifetime;
    bool active;
};

Projectile* projectile_create(Vector2 position, Vector2 direction) {
    Projectile* projectile = (Projectile*)malloc(sizeof(Projectile));
    if (!projectile) return NULL;
    
    projectile->position = position;
    projectile->radius = PROJECTILE_RADIUS;
    projectile->damage = PROJECTILE_DAMAGE;
    projectile->max_lifetime = PROJECTILE_LIFETIME;
    projectile->lifetime = PROJECTILE_LIFETIME;
    projectile->active = true;
    
    // Normalize direction and set velocity
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f) {
        projectile->velocity.x = (direction.x / length) * PROJECTILE_SPEED;
        projectile->velocity.y = (direction.y / length) * PROJECTILE_SPEED;
    } else {
        // Default direction if zero vector
        projectile->velocity.x = PROJECTILE_SPEED;
        projectile->velocity.y = 0.0f;
    }
    
    return projectile;
}

void projectile_destroy(Projectile* projectile) {
    if (projectile) free(projectile);
}

Vector2 projectile_get_position(const Projectile* projectile) {
    if (!projectile) return (Vector2){0, 0};
    return projectile->position;
}

Vector2 projectile_get_velocity(const Projectile* projectile) {
    if (!projectile) return (Vector2){0, 0};
    return projectile->velocity;
}

float projectile_get_radius(const Projectile* projectile) {
    if (!projectile) return 0.0f;
    return projectile->radius;
}

float projectile_get_damage(const Projectile* projectile) {
    if (!projectile) return 0.0f;
    return projectile->damage;
}

bool projectile_is_active(const Projectile* projectile) {
    if (!projectile) return false;
    return projectile->active && projectile->lifetime > 0;
}

void projectile_update(Projectile* projectile) {
    if (!projectile || !projectile->active) return;
    
    projectile->lifetime--;
    if (projectile->lifetime <= 0) {
        projectile->active = false;
        return;
    }
    
    projectile->position.x += projectile->velocity.x;
    projectile->position.y += projectile->velocity.y;
    
    // Deactivate if out of bounds
    #define SCREEN_WIDTH 800
    #define SCREEN_HEIGHT 600
    if (projectile->position.x < -projectile->radius || 
        projectile->position.x > SCREEN_WIDTH + projectile->radius ||
        projectile->position.y < -projectile->radius || 
        projectile->position.y > SCREEN_HEIGHT + projectile->radius) {
        projectile->active = false;
    }
}

bool projectile_check_circle_collision(const Projectile* projectile, Vector2 circle_pos, float circle_radius) {
    if (!projectile || !projectile->active) return false;
    
    float dx = projectile->position.x - circle_pos.x;
    float dy = projectile->position.y - circle_pos.y;
    float distance = sqrtf(dx * dx + dy * dy);
    
    return distance < (projectile->radius + circle_radius);
}

bool projectile_check_rect_collision(const Projectile* projectile, Rectangle rect) {
    if (!projectile || !projectile->active) return false;
    
    // Find closest point on rectangle to projectile center
    float closest_x = fmaxf(rect.x, fminf(projectile->position.x, rect.x + rect.width));
    float closest_y = fmaxf(rect.y, fminf(projectile->position.y, rect.y + rect.height));
    
    float dx = projectile->position.x - closest_x;
    float dy = projectile->position.y - closest_y;
    float distance_sq = dx * dx + dy * dy;
    
    return distance_sq < (projectile->radius * projectile->radius);
}

