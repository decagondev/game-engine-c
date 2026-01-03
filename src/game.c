#include "../include/game.h"
#include "../include/gengine.h"
#include "../include/audio.h"
#include "../include/map.h"
#include "../include/renderer.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/item.h"
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define NUM_MAPS 4
#define OBSTACLE_SPEED 2.0f
#define OBSTACLE_DIRECTION_CHANGE_FRAMES 120

#define MAX_HIGH_SCORES 10
#define HIGH_SCORE_FILENAME "highscores.txt"
#define MAX_NAME_LENGTH 20

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int frame_count;
    int coins_collected;
    float health_remaining;
} HighScore;

typedef enum {
    GAME_STATE_START,
    GAME_STATE_PLAYING,
    GAME_STATE_END,
    GAME_STATE_ENTER_NAME,
    GAME_STATE_HIGH_SCORES
} GameStateType;

typedef struct {
    bool running;
    int frame_count;
    int game_start_frame;
    Player* player;
    int current_map_id;
    int coins_collected;
    int total_coins;
    GameStateType state;
    Map maps[NUM_MAPS];
    HighScore high_scores[MAX_HIGH_SCORES];
    int high_score_count;
    char player_name[MAX_NAME_LENGTH + 1];
    int name_char_count;
    HighScore pending_score;
} GameState;

struct CoinCollectorGame {
    GameState state;
    GameEngine* engine;
};

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_TITLE "Game Engine"
#define PLAYER_SPEED 5.0f
#define PLAYER_RADIUS 25.0f
#define EXIT_WIDTH 60.0f
#define EXIT_HEIGHT 60.0f
#define MAX_HEALTH 100.0f
#define DAMAGE_PER_HIT 10.0f
#define INVINCIBILITY_FRAMES 60

/**
 * Check if all coins have been collected.
 * @param game Game state
 * @return true if all coins collected, false otherwise
 */
bool all_coins_collected(GameState* game) {
    int total_collected = 0;
    for (int i = 0; i < NUM_MAPS; i++) {
        for (int j = 0; j < game->maps[i].coin_count; j++) {
            if (game->maps[i].coins[j].collected) {
                total_collected++;
            }
        }
    }
    return total_collected >= game->total_coins;
}

/**
 * Load high scores from file.
 * @param game Game state
 */
void load_high_scores(GameState* game) {
    game->high_score_count = 0;
    
    FILE* file = fopen(HIGH_SCORE_FILENAME, "r");
    if (file == NULL) {
        return;
    }
    
    HighScore score;
    char line[256];
    
    while (game->high_score_count < MAX_HIGH_SCORES && fgets(line, sizeof(line), file) != NULL) {
        char name_buffer[MAX_NAME_LENGTH + 1] = {0};
        if (sscanf(line, "%20s %d %d %f", name_buffer, &score.frame_count, &score.coins_collected, &score.health_remaining) == 4) {
            strncpy(score.name, name_buffer, MAX_NAME_LENGTH);
            score.name[MAX_NAME_LENGTH] = '\0';
            game->high_scores[game->high_score_count++] = score;
        }
    }
    
    fclose(file);
}

/**
 * Save high scores to file.
 * @param game Game state
 */
void save_high_scores(GameState* game) {
    FILE* file = fopen(HIGH_SCORE_FILENAME, "w");
    if (file == NULL) {
        printf("Error: Could not save high scores to file\n");
        return;
    }
    
    for (int i = 0; i < game->high_score_count; i++) {
        fprintf(file, "%s %d %d %.1f\n", 
                game->high_scores[i].name,
                game->high_scores[i].frame_count,
                game->high_scores[i].coins_collected,
                game->high_scores[i].health_remaining);
    }
    
    fclose(file);
}

/**
 * Add a new high score if it qualifies.
 * @param game Game state
 * @param name Player name
 * @param frame_count Completion frame count
 * @param coins_collected Number of coins collected
 * @param health_remaining Remaining health
 */
void add_high_score(GameState* game, const char* name, int frame_count, int coins_collected, float health_remaining) {
    HighScore new_score;
    strncpy(new_score.name, name, MAX_NAME_LENGTH);
    new_score.name[MAX_NAME_LENGTH] = '\0';
    new_score.frame_count = frame_count;
    new_score.coins_collected = coins_collected;
    new_score.health_remaining = health_remaining;
    
    int insert_pos = game->high_score_count;
    for (int i = 0; i < game->high_score_count; i++) {
        if (frame_count < game->high_scores[i].frame_count) {
            insert_pos = i;
            break;
        }
    }
    
    if (game->high_score_count < MAX_HIGH_SCORES || insert_pos < MAX_HIGH_SCORES) {
        if (game->high_score_count >= MAX_HIGH_SCORES) {
            game->high_score_count = MAX_HIGH_SCORES - 1;
        }
        
        for (int i = game->high_score_count; i > insert_pos; i--) {
            game->high_scores[i] = game->high_scores[i - 1];
        }
        
        game->high_scores[insert_pos] = new_score;
        game->high_score_count++;
        
        save_high_scores(game);
        printf("New high score added! Rank: %d, Name: %s, Frames: %d\n", insert_pos + 1, name, frame_count);
    }
}

/**
 * Game initialization callback.
 * @param game_data Game data pointer
 */
static void game_init_callback(void* game_data) {
    CoinCollectorGame* game = (CoinCollectorGame*)game_data;
    GameState* state = &game->state;
    
    state->running = true;
    state->frame_count = 0;
    state->game_start_frame = 0;
    state->current_map_id = 0;
    state->coins_collected = 0;
    state->state = GAME_STATE_START;
    state->high_score_count = 0;
    memset(state->player_name, 0, sizeof(state->player_name));
    state->name_char_count = 0;
    memset(&state->pending_score, 0, sizeof(state->pending_score));
    
    state->player = player_create();
    if (!state->player) {
        printf("Error: Failed to create player\n");
        return;
    }
    
    load_high_scores(state);
    
    for (int i = 0; i < NUM_MAPS; i++) {
        map_init(&state->maps[i], i);
    }
    
    state->total_coins = 0;
    for (int i = 0; i < NUM_MAPS; i++) {
        state->total_coins += state->maps[i].coin_count;
    }
    
    Vector2 start_pos = state->maps[0].entrances[0].position;
    player_init(state->player, start_pos);
    
    printf("Game initialized! Total coins: %d\n", state->total_coins);
    printf("High scores loaded: %d\n", state->high_score_count);
}

/**
 * Game update callback.
 * @param game_data Game data pointer
 * @param delta_time Time since last frame
 */
static void game_update_callback(void* game_data, float delta_time) {
    CoinCollectorGame* game = (CoinCollectorGame*)game_data;
    GameState* state = &game->state;
    
    if (game->engine) {
        state->frame_count = gengine_get_frame_count(game->engine);
    } else {
        state->frame_count++;
    }
    
    if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
        state->running = false;
        if (game->engine) {
            gengine_stop(game->engine);
        }
    }
    
    if (state->state == GAME_STATE_START) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state->state = GAME_STATE_PLAYING;
            state->game_start_frame = state->frame_count;
        }
        if (IsKeyPressed(KEY_H)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state->state = GAME_STATE_HIGH_SCORES;
        }
        return;
    }
    
    if (state->state == GAME_STATE_HIGH_SCORES) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_H) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            state->state = GAME_STATE_START;
        }
        return;
    }
    
    if (state->state == GAME_STATE_ENTER_NAME) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (state->name_char_count < MAX_NAME_LENGTH)) {
                state->player_name[state->name_char_count] = (char)key;
                state->name_char_count++;
                state->player_name[state->name_char_count] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (state->name_char_count > 0) {
                state->name_char_count--;
                state->player_name[state->name_char_count] = '\0';
            }
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            audio_play_sound(AUDIO_SOUND_MENU);
            if (state->name_char_count > 0) {
                add_high_score(state, state->player_name, 
                              state->pending_score.frame_count,
                              state->pending_score.coins_collected,
                              state->pending_score.health_remaining);
                state->state = GAME_STATE_END;
            } else {
                add_high_score(state, "Player", 
                              state->pending_score.frame_count,
                              state->pending_score.coins_collected,
                              state->pending_score.health_remaining);
                state->state = GAME_STATE_END;
            }
        }
        return;
    }
    
    if (state->state == GAME_STATE_END) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            state->state = GAME_STATE_START;
            state->coins_collected = 0;
            state->current_map_id = 0;
            Vector2 start_pos = state->maps[0].entrances[0].position;
            player_reset(state->player, start_pos);
            state->game_start_frame = 0;
            for (int i = 0; i < NUM_MAPS; i++) {
                for (int j = 0; j < state->maps[i].coin_count; j++) {
                    state->maps[i].coins[j].collected = false;
                }
            }
        }
        return;
    }
    
    if (!state->player) return;
    Map* current_map = &state->maps[state->current_map_id];
    
    player_update(state->player);
    
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
    
    player_update_movement(state->player, current_map);
    
    int target_map_id, target_entrance_id;
    if (player_check_exit_collision(state->player, current_map, &target_map_id, &target_entrance_id)) {
        state->current_map_id = target_map_id;
        Map* target_map = &state->maps[target_map_id];
        
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
        player_set_position(state->player, new_pos);
        
        printf("Entered map %d\n", target_map_id);
    }
    
    Vector2 player_pos = player_get_position(state->player);
    for (int i = 0; i < current_map->obstacle_count; i++) {
        Obstacle* obstacle = &current_map->obstacles[i];
        
        Enemy* enemy = enemy_create(obstacle->position, obstacle->velocity, obstacle->color);
        if (enemy) {
            if (enemy_check_collision_with_player(enemy, player_pos, PLAYER_RADIUS) && !player_is_invincible(state->player)) {
                player_apply_damage(state->player, DAMAGE_PER_HIT);
                audio_play_sound(AUDIO_SOUND_DAMAGE);
                printf("Hit! Health: %.0f/%.0f\n", player_get_health(state->player), player_get_max_health(state->player));
                
                if (!player_is_alive(state->player)) {
                    printf("Player died! Resetting game...\n");
                    state->coins_collected = 0;
                    state->current_map_id = 0;
                    Vector2 start_pos = state->maps[0].entrances[0].position;
                    player_reset(state->player, start_pos);
                    state->game_start_frame = state->frame_count;
                    for (int j = 0; j < NUM_MAPS; j++) {
                        for (int k = 0; k < state->maps[j].coin_count; k++) {
                            state->maps[j].coins[k].collected = false;
                        }
                    }
                    for (int j = 0; j < NUM_MAPS; j++) {
                        map_init(&state->maps[j], j);
                    }
                    enemy_destroy(enemy);
                    break;
                }
            }
            enemy_destroy(enemy);
        }
    }
    
    player_pos = player_get_position(state->player);
    for (int i = 0; i < current_map->coin_count; i++) {
        Coin* coin = &current_map->coins[i];
        if (!coin->collected) {
            Item* item = item_create(ITEM_TYPE_COIN, coin->position);
            if (item) {
                if (item_check_collision_with_player(item, player_pos, PLAYER_RADIUS)) {
                    item_collect(item);
                    coin->collected = true;
                    state->coins_collected++;
                    audio_play_sound(AUDIO_SOUND_COIN);
                    printf("Coin collected! Total: %d/%d\n", state->coins_collected, state->total_coins);
                    
                    if (all_coins_collected(state)) {
                        int completion_frames = state->frame_count - state->game_start_frame;
                        printf("All coins collected! Game complete in %d frames!\n", completion_frames);
                        
                        audio_play_sound(AUDIO_SOUND_VICTORY);
                        
                        state->pending_score.frame_count = completion_frames;
                        state->pending_score.coins_collected = state->coins_collected;
                        state->pending_score.health_remaining = player_get_health(state->player);
                        
                        memset(state->player_name, 0, sizeof(state->player_name));
                        state->name_char_count = 0;
                        
                        state->state = GAME_STATE_ENTER_NAME;
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
    GameState* state = &game->state;
    
    if (state->state == GAME_STATE_START) {
        renderer_draw_start_screen(state->frame_count, state->high_scores, state->high_score_count);
        return;
    }
    
    if (state->state == GAME_STATE_END) {
        if (!state->player) return;
        renderer_draw_end_screen(state->frame_count, state->game_start_frame, state->total_coins, 
                                 player_get_health(state->player), player_get_max_health(state->player), 
                                 state->high_scores, state->high_score_count);
        return;
    }
    
    if (state->state == GAME_STATE_ENTER_NAME) {
        renderer_draw_name_entry_screen(state->player_name, state->name_char_count, 
                                       state->pending_score.frame_count, 
                                       state->pending_score.coins_collected,
                                       state->pending_score.health_remaining);
        return;
    }
    
    if (state->state == GAME_STATE_HIGH_SCORES) {
        renderer_draw_high_scores_screen(state->high_scores, state->high_score_count);
        return;
    }
    
    if (!state->player) return;
    Map* current_map = &state->maps[state->current_map_id];
    renderer_draw_game_screen(current_map, player_get_position(state->player), 
                             player_is_invincible(state->player), player_get_invincibility_timer(state->player),
                             player_get_health(state->player), player_get_max_health(state->player), 
                             state->current_map_id, state->coins_collected);
}

/**
 * Game cleanup callback.
 * @param game_data Game data pointer
 */
static void game_cleanup_callback(void* game_data) {
    CoinCollectorGame* game = (CoinCollectorGame*)game_data;
    GameState* state = &game->state;
    
    if (state->player) {
        player_destroy(state->player);
        state->player = NULL;
    }
    
    printf("Game cleanup. Total frames: %d\n", state->frame_count);
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
    
    return game;
}

void game_destroy(CoinCollectorGame* game) {
    if (!game) return;
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
