#include "../include/game.h"
#include "../include/gengine.h"
#include "../include/audio.h"
#include "../include/map.h"
#include "../include/renderer.h"
#include "../include/renderer3d.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/item.h"
#include "../include/state.h"
#include "../include/highscore.h"
#include "../include/projectile.h"
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_RADIUS 25.0f
#define DAMAGE_PER_HIT 10.0f
#define PROJECTILE_COOLDOWN 10  // frames between shots
#define OBSTACLE_RADIUS 20.0f

// Shared static variable for mode selection (shared between update and render callbacks)
static int g_selected_mode = 0;

struct CoinCollectorGame {
    GameState* state;
    GameEngine* engine;
};

/**
 * Game initialization callback.
 * @param game_data Game data pointer
 */
static void game_init_callback(void* game_data) {
    CoinCollectorGame* game = (CoinCollectorGame*)game_data;
    if (!game) return;
    
    game->state = state_create();
    if (!game->state) {
        printf("Error: Failed to create game state\n");
        return;
    }
    
    state_init(game->state);
}

/**
 * Game update callback.
 * @param game_data Game data pointer
 * @param delta_time Time since last frame
 */
static void game_update_callback(void* game_data, float delta_time) {
    CoinCollectorGame* game = (CoinCollectorGame*)game_data;
    if (!game || !game->state) return;
    
    GameState* state = game->state;
    
    if (game->engine) {
        state_set_frame_count(state, gengine_get_frame_count(game->engine));
    } else {
        state_increment_frame_count(state);
    }
    
    if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
        state_set_running(state, false);
        if (game->engine) {
            gengine_stop(game->engine);
        }
    }
    
    GameStateType current_state = state_get_type(state);
    
    if (current_state == GAME_STATE_START) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state_set_type(state, GAME_STATE_MODE_SELECT);
        }
        if (IsKeyPressed(KEY_H)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state_set_type(state, GAME_STATE_HIGH_SCORES);
        }
        return;
    }
    
    if (current_state == GAME_STATE_MODE_SELECT) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            g_selected_mode = 0;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            g_selected_mode = 1;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state_set_game_mode(state, (g_selected_mode == 0) ? GAME_MODE_2D : GAME_MODE_3D);
            state_set_type(state, GAME_STATE_PLAYING);
            state_set_game_start_frame(state, state_get_frame_count(state));
            g_selected_mode = 0;  // Reset for next time
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state_set_type(state, GAME_STATE_START);
            g_selected_mode = 0;  // Reset for next time
        }
        return;
    }
    
    if (current_state == GAME_STATE_HIGH_SCORES) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_H) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state_set_type(state, GAME_STATE_START);
        }
        return;
    }
    
    if (current_state == GAME_STATE_ENTER_NAME) {
        char* player_name = state_get_player_name(state);
        int name_char_count = state_get_name_char_count(state);
        
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (name_char_count < MAX_NAME_LENGTH)) {
                player_name[name_char_count] = (char)key;
                state_increment_name_char_count(state);
                name_char_count = state_get_name_char_count(state);
                player_name[name_char_count] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (name_char_count > 0) {
                state_decrement_name_char_count(state);
                name_char_count = state_get_name_char_count(state);
                player_name[name_char_count] = '\0';
            }
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            HighScore* pending_score = state_get_pending_score(state);
            int* high_score_count;
            HighScore* high_scores = state_get_high_scores_mutable(state, &high_score_count);
            
            if (name_char_count > 0) {
                highscore_add(high_scores, high_score_count, player_name,
                             pending_score->frame_count,
                             pending_score->coins_collected,
                             pending_score->health_remaining);
            } else {
                highscore_add(high_scores, high_score_count, "Player",
                             pending_score->frame_count,
                             pending_score->coins_collected,
                             pending_score->health_remaining);
            }
            state_set_type(state, GAME_STATE_END);
        }
        return;
    }
    
    if (current_state == GAME_STATE_END) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            state_set_type(state, GAME_STATE_START);
            state_reset(state);
        }
        return;
    }
    
    Player* player = state_get_player(state);
    if (!player) return;
    
    Map* current_map = state_get_current_map(state);
    if (!current_map) return;
    
    player_update(player);
    
    // Handle 3D mode rotation and movement
    GameMode mode = state_get_game_mode(state);
    if (mode == GAME_MODE_3D) {
        // Mouse look for smooth rotation
        static Vector2 last_mouse_pos = {0, 0};
        Vector2 current_mouse_pos = GetMousePosition();
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            float mouse_delta_x = current_mouse_pos.x - last_mouse_pos.x;
            float rotation_sensitivity = 0.003f;
            float new_angle = player_get_angle(player) + mouse_delta_x * rotation_sensitivity;
            player_set_angle(player, new_angle);
        }
        last_mouse_pos = current_mouse_pos;
        
        // Keyboard rotation (alternative)
        float rotation_speed = 0.05f;
        if (IsKeyDown(KEY_Q)) {
            float new_angle = player_get_angle(player) - rotation_speed;
            player_set_angle(player, new_angle);
        }
        if (IsKeyDown(KEY_E)) {
            float new_angle = player_get_angle(player) + rotation_speed;
            player_set_angle(player, new_angle);
        }
        
        // 3D movement (forward/backward relative to view angle)
        Vector2 movement = {0.0f, 0.0f};
        float angle = player_get_angle(player);
        float speed = PLAYER_SPEED;
        
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            movement.x += cosf(angle) * speed;
            movement.y += sinf(angle) * speed;
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            movement.x -= cosf(angle) * speed;
            movement.y -= sinf(angle) * speed;
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            // Strafe left
            movement.x += cosf(angle - PI/2) * speed;
            movement.y += sinf(angle - PI/2) * speed;
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            // Strafe right
            movement.x += cosf(angle + PI/2) * speed;
            movement.y += sinf(angle + PI/2) * speed;
        }
        
        Vector2 current_pos = player_get_position(player);
        Vector2 new_position = {
            current_pos.x + movement.x,
            current_pos.y + movement.y
        };
        
        if (!player_check_wall_collision(player, new_position, current_map)) {
            player_set_position(player, new_position);
        }
        
        // Boundary checks
        Vector2 pos = player_get_position(player);
        if (pos.x < PLAYER_RADIUS) player_set_position(player, (Vector2){PLAYER_RADIUS, pos.y});
        if (pos.x > SCREEN_WIDTH - PLAYER_RADIUS) player_set_position(player, (Vector2){SCREEN_WIDTH - PLAYER_RADIUS, pos.y});
        if (pos.y < PLAYER_RADIUS) player_set_position(player, (Vector2){pos.x, PLAYER_RADIUS});
        if (pos.y > SCREEN_HEIGHT - PLAYER_RADIUS) player_set_position(player, (Vector2){pos.x, SCREEN_HEIGHT - PLAYER_RADIUS});
    } else {
        // 2D mode movement (existing code)
        player_update_movement(player, current_map);
    }
    
    // Handle shooting input (only in 2D mode)
    if (mode == GAME_MODE_2D) {
        state_decrement_projectile_cooldown(state);
        Vector2 player_pos = player_get_position(player);
        
        if (state_get_projectile_cooldown(state) <= 0) {
            Vector2 shoot_direction = {0, 0};
            bool should_shoot = false;
            
            // Mouse shooting (primary method)
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse_pos = GetMousePosition();
                shoot_direction.x = mouse_pos.x - player_pos.x;
                shoot_direction.y = mouse_pos.y - player_pos.y;
                should_shoot = true;
            }
            // Arrow key shooting (alternative)
            else if (IsKeyPressed(KEY_SPACE)) {
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
                    shoot_direction = (Vector2){0, -1};
                    should_shoot = true;
                } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
                    shoot_direction = (Vector2){0, 1};
                    should_shoot = true;
                } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                    shoot_direction = (Vector2){-1, 0};
                    should_shoot = true;
                } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                    shoot_direction = (Vector2){1, 0};
                    should_shoot = true;
                }
            }
            
            if (should_shoot) {
                float length = sqrtf(shoot_direction.x * shoot_direction.x + shoot_direction.y * shoot_direction.y);
                if (length > 0.1f) {  // Only shoot if direction is meaningful
                    Projectile* proj = projectile_create(player_pos, shoot_direction);
                    if (proj && state_add_projectile(state, proj)) {
                        state_set_projectile_cooldown(state, PROJECTILE_COOLDOWN);
                        audio_play_sound(AUDIO_SOUND_MENU);  // Use menu sound for shooting
                    } else if (proj) {
                        projectile_destroy(proj);  // Clean up if couldn't add
                    }
                }
            }
        }
        
        // Update projectiles
        int projectile_count;
        Projectile** projectiles = state_get_projectiles(state, &projectile_count);
        for (int i = projectile_count - 1; i >= 0; i--) {
            if (!projectiles[i] || !projectile_is_active(projectiles[i])) {
                if (projectiles[i]) {
                    state_remove_projectile(state, i);
                }
                continue;
            }
            
            projectile_update(projectiles[i]);
            
            if (!projectile_is_active(projectiles[i])) {
                state_remove_projectile(state, i);
                continue;
            }
            
            // Check collision with walls
            int wall_count;
            const Wall* walls = map_get_walls(current_map, &wall_count);
            bool hit_wall = false;
            for (int j = 0; j < wall_count; j++) {
                if (projectile_check_rect_collision(projectiles[i], walls[j].rect)) {
                    hit_wall = true;
                    break;
                }
            }
            
            if (hit_wall) {
                state_remove_projectile(state, i);
                continue;
            }
            
            // Check collision with enemies
            Vector2 proj_pos = projectile_get_position(projectiles[i]);
            for (int j = 0; j < current_map->obstacle_count; j++) {
                Obstacle* obstacle = &current_map->obstacles[j];
                if (projectile_check_circle_collision(projectiles[i], obstacle->position, OBSTACLE_RADIUS)) {
                    // Damage enemy (for now, just remove projectile)
                    // In the future, we could add enemy health system
                    state_remove_projectile(state, i);
                    break;
                }
            }
        }
    }
    
    for (int i = 0; i < current_map->obstacle_count; i++) {
        Obstacle* obstacle = &current_map->obstacles[i];
        
        Enemy* enemy = enemy_create(obstacle->position, obstacle->velocity, obstacle->color);
        if (enemy) {
            enemy_set_direction_timer(enemy, obstacle->direction_change_timer);
            enemy_update(enemy, current_map);
            
            obstacle->position = enemy_get_position(enemy);
            obstacle->velocity = enemy_get_velocity(enemy);
            obstacle->direction_change_timer = enemy_get_direction_timer(enemy);
            
            enemy_destroy(enemy);
        }
    }
    
    // Handle map transitions (works in both modes)
    Vector2 player_pos = player_get_position(player);
    int target_map_id, target_entrance_id;
    if (player_check_exit_collision(player, current_map, &target_map_id, &target_entrance_id)) {
        state_set_current_map_id(state, target_map_id);
        Map* target_map = &state_get_maps(state)[target_map_id];
        
        int entrance_count;
        const Entrance* entrances = map_get_entrances(target_map, &entrance_count);
        Vector2 new_pos;
        if (target_entrance_id < entrance_count) {
            new_pos = entrances[target_entrance_id].position;
        } else if (entrance_count > 0) {
            new_pos = entrances[0].position;
        } else {
            new_pos = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
        }
        player_set_position(player, new_pos);
        
        printf("Entered map %d\n", target_map_id);
        player_pos = player_get_position(player);  // Update after map change
    }
    
    // Enemy collision detection (works in both modes)
    for (int i = 0; i < current_map->obstacle_count; i++) {
        Obstacle* obstacle = &current_map->obstacles[i];
        
        Enemy* enemy = enemy_create(obstacle->position, obstacle->velocity, obstacle->color);
        if (enemy) {
            if (enemy_check_collision_with_player(enemy, player_pos, PLAYER_RADIUS) && !player_is_invincible(player)) {
                player_apply_damage(player, DAMAGE_PER_HIT);
                audio_play_sound(AUDIO_SOUND_DAMAGE);
                printf("Hit! Health: %.0f/%.0f\n", player_get_health(player), player_get_max_health(player));
                
                if (!player_is_alive(player)) {
                    printf("Player died! Resetting game...\n");
                    state_reset(state);
                    state_set_game_start_frame(state, state_get_frame_count(state));
                    
                    Map* maps = state_get_maps(state);
                    for (int j = 0; j < NUM_MAPS; j++) {
                        map_init(&maps[j], j);
                    }
                    enemy_destroy(enemy);
                    break;
                }
            }
            enemy_destroy(enemy);
        }
    }
    
    player_pos = player_get_position(player);
    for (int i = 0; i < current_map->coin_count; i++) {
        Coin* coin = &current_map->coins[i];
        if (!coin->collected) {
            Item* item = item_create(ITEM_TYPE_COIN, coin->position);
            if (item) {
                if (item_check_collision_with_player(item, player_pos, PLAYER_RADIUS)) {
                    item_collect(item);
                    coin->collected = true;
                    state_increment_coins_collected(state);
                    audio_play_sound(AUDIO_SOUND_COIN);
                    printf("Coin collected! Total: %d/%d\n", 
                           state_get_coins_collected(state), state_get_total_coins(state));
                    
                    if (state_all_coins_collected(state)) {
                        int completion_frames = state_get_frame_count(state) - state_get_game_start_frame(state);
                        printf("All coins collected! Game complete in %d frames!\n", completion_frames);
                        
                        audio_play_sound(AUDIO_SOUND_VICTORY);
                        
                        HighScore* pending_score = state_get_pending_score(state);
                        pending_score->frame_count = completion_frames;
                        pending_score->coins_collected = state_get_coins_collected(state);
                        pending_score->health_remaining = player_get_health(player);
                        
                        char* player_name = state_get_player_name(state);
                        memset(player_name, 0, MAX_NAME_LENGTH + 1);
                        state_set_name_char_count(state, 0);
                        
                        state_set_type(state, GAME_STATE_ENTER_NAME);
                    }
                }
                item_destroy(item);
            }
        }
    }
}

/**
 * Game render callback.
 * @param game_data Game data pointer
 */
static void game_render_callback(void* game_data) {
    CoinCollectorGame* game = (CoinCollectorGame*)game_data;
    if (!game || !game->state) return;
    
    GameState* state = game->state;
    GameStateType current_state = state_get_type(state);
    
    if (current_state == GAME_STATE_START) {
        int high_score_count;
        const HighScore* high_scores = state_get_high_scores(state, &high_score_count);
        renderer_draw_start_screen(state_get_frame_count(state), high_scores, high_score_count);
        return;
    }
    
    if (current_state == GAME_STATE_MODE_SELECT) {
        renderer_draw_mode_select_screen(state_get_frame_count(state), g_selected_mode);
        return;
    }
    
    if (current_state == GAME_STATE_END) {
        Player* player = state_get_player(state);
        if (!player) return;
        
        int high_score_count;
        const HighScore* high_scores = state_get_high_scores(state, &high_score_count);
        renderer_draw_end_screen(state_get_frame_count(state), state_get_game_start_frame(state),
                                 state_get_total_coins(state),
                                 player_get_health(player), player_get_max_health(player),
                                 high_scores, high_score_count);
        return;
    }
    
    if (current_state == GAME_STATE_ENTER_NAME) {
        HighScore* pending_score = state_get_pending_score(state);
        renderer_draw_name_entry_screen(state_get_player_name(state), 
                                       state_get_name_char_count(state),
                                       pending_score->frame_count,
                                       pending_score->coins_collected,
                                       pending_score->health_remaining);
        return;
    }
    
    if (current_state == GAME_STATE_HIGH_SCORES) {
        int high_score_count;
        const HighScore* high_scores = state_get_high_scores(state, &high_score_count);
        renderer_draw_high_scores_screen(high_scores, high_score_count);
        return;
    }
    
    Player* player = state_get_player(state);
    if (!player) return;
    
    Map* current_map = state_get_current_map(state);
    if (!current_map) return;
    
    GameMode mode = state_get_game_mode(state);
    
    if (mode == GAME_MODE_3D) {
        // Collect enemy positions and colors for 3D rendering
        Vector2 enemy_positions[50];
        Color enemy_colors[50];
        int enemy_count = 0;
        
        for (int i = 0; i < current_map->obstacle_count && enemy_count < 50; i++) {
            enemy_positions[enemy_count] = current_map->obstacles[i].position;
            enemy_colors[enemy_count] = current_map->obstacles[i].color;
            enemy_count++;
        }
        
        // Collect coin positions and collected status for 3D rendering
        Vector2 coin_positions[50];
        bool coin_collected[50];
        int coin_count = 0;
        
        for (int i = 0; i < current_map->coin_count && coin_count < 50; i++) {
            coin_positions[coin_count] = current_map->coins[i].position;
            coin_collected[coin_count] = current_map->coins[i].collected;
            coin_count++;
        }
        
        // Render 3D view
        renderer3d_render(current_map, player_get_position(player), player_get_angle(player),
                         player_get_health(player), player_get_max_health(player),
                         state_get_current_map_id(state), state_get_coins_collected(state),
                         enemy_positions, enemy_count, enemy_colors,
                         coin_positions, coin_count, coin_collected);
    } else {
        // Render 2D view
        int projectile_count;
        Projectile** projectiles = state_get_projectiles(state, &projectile_count);
        
        renderer_draw_game_screen(current_map, player_get_position(player),
                                 player_is_invincible(player), player_get_invincibility_timer(player),
                                 player_get_health(player), player_get_max_health(player),
                                 state_get_current_map_id(state), state_get_coins_collected(state),
                                 projectiles, projectile_count);
    }
}

/**
 * Game cleanup callback.
 * @param game_data Game data pointer
 */
static void game_cleanup_callback(void* game_data) {
    CoinCollectorGame* game = (CoinCollectorGame*)game_data;
    if (!game) return;
    
    if (game->state) {
        printf("Game cleanup. Total frames: %d\n", state_get_frame_count(game->state));
        state_destroy(game->state);
        game->state = NULL;
    }
}

/**
 * Game input handler callback.
 * @param game_data Game data pointer
 * @param key Key pressed
 */
static void game_handle_input_callback(void* game_data, int key) {
    (void)game_data;
    (void)key;
}

CoinCollectorGame* game_create(void) {
    CoinCollectorGame* game = (CoinCollectorGame*)malloc(sizeof(CoinCollectorGame));
    if (!game) {
        return NULL;
    }
    
    memset(game, 0, sizeof(CoinCollectorGame));
    game->engine = NULL;
    game->state = NULL;
    
    return game;
}

void game_destroy(CoinCollectorGame* game) {
    if (!game) return;
    
    if (game->state) {
        state_destroy(game->state);
    }
    free(game);
}

GameCallbacks game_get_callbacks(CoinCollectorGame* game) {
    GameCallbacks callbacks = {0};
    if (game) {
        callbacks.init = game_init_callback;
        callbacks.update = game_update_callback;
        callbacks.render = game_render_callback;
        callbacks.cleanup = game_cleanup_callback;
        callbacks.handle_input = game_handle_input_callback;
    }
    return callbacks;
}

void* game_get_data(CoinCollectorGame* game) {
    return (void*)game;
}

void game_set_engine(CoinCollectorGame* game, GameEngine* engine) {
    if (game) {
        game->engine = engine;
    }
}
