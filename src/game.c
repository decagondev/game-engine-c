#include "../include/game.h"
#include "../include/gengine.h"
#include "../include/audio.h"
#include "../include/map.h"
#include "../include/renderer.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/item.h"
#include "../include/state.h"
#include "../include/highscore.h"
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
            state_set_type(state, GAME_STATE_PLAYING);
            state_set_game_start_frame(state, state_get_frame_count(state));
        }
        if (IsKeyPressed(KEY_H)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state_set_type(state, GAME_STATE_HIGH_SCORES);
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
    
    player_update_movement(player, current_map);
    
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
    }
    
    Vector2 player_pos = player_get_position(player);
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
    
    renderer_draw_game_screen(current_map, player_get_position(player),
                             player_is_invincible(player), player_get_invincibility_timer(player),
                             player_get_health(player), player_get_max_health(player),
                             state_get_current_map_id(state), state_get_coins_collected(state));
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
