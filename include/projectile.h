#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"
#include <stdbool.h>

#define PROJECTILE_SPEED 10.0f
#define PROJECTILE_RADIUS 5.0f
#define PROJECTILE_DAMAGE 20.0f
#define PROJECTILE_LIFETIME 120  // frames

typedef struct Projectile Projectile;

/**
 * Create a new projectile instance.
 * @param position Starting position
 * @param direction Direction vector (will be normalized)
 * @return Pointer to created projectile, or NULL on failure
 */
Projectile* projectile_create(Vector2 position, Vector2 direction);

/**
 * Destroy a projectile instance.
 * @param projectile The projectile to destroy
 */
void projectile_destroy(Projectile* projectile);

/**
 * Get the projectile's position.
 * @param projectile The projectile
 * @return Current position
 */
Vector2 projectile_get_position(const Projectile* projectile);

/**
 * Get the projectile's velocity.
 * @param projectile The projectile
 * @return Current velocity
 */
Vector2 projectile_get_velocity(const Projectile* projectile);

/**
 * Get the projectile's radius.
 * @param projectile The projectile
 * @return Radius
 */
float projectile_get_radius(const Projectile* projectile);

/**
 * Get the projectile's damage.
 * @param projectile The projectile
 * @return Damage value
 */
float projectile_get_damage(const Projectile* projectile);

/**
 * Check if the projectile is active (not expired).
 * @param projectile The projectile
 * @return true if active, false otherwise
 */
bool projectile_is_active(const Projectile* projectile);

/**
 * Update projectile position and lifetime.
 * @param projectile The projectile
 */
void projectile_update(Projectile* projectile);

/**
 * Check if projectile collides with a circle.
 * @param projectile The projectile
 * @param circle_pos Center of the circle
 * @param circle_radius Radius of the circle
 * @return true if collision detected, false otherwise
 */
bool projectile_check_circle_collision(const Projectile* projectile, Vector2 circle_pos, float circle_radius);

/**
 * Check if projectile collides with a rectangle.
 * @param projectile The projectile
 * @param rect Rectangle to check
 * @return true if collision detected, false otherwise
 */
bool projectile_check_rect_collision(const Projectile* projectile, Rectangle rect);

#endif

