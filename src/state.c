#include "../include/state.h"
#include "../include/map.h"
#include "../include/player.h"
#include "../include/highscore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct GameState {
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
};

GameState* state_create(void) {
    GameState* state = (GameState*)malloc(sizeof(GameState));
    if (!state) return NULL;
    memset(state, 0, sizeof(GameState));
    return state;
}

void state_destroy(GameState* state) {
    if (!state) return;
    if (state->player) {
        player_destroy(state->player);
    }
    free(state);
}

void state_init(GameState* state) {
    if (!state) return;
    
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
    
    highscore_load(state->high_scores, &state->high_score_count);
    
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

void state_reset(GameState* state) {
    if (!state) return;
    
    state->coins_collected = 0;
    state->current_map_id = 0;
    Vector2 start_pos = state->maps[0].entrances[0].position;
    player_reset(state->player, start_pos);
    state->game_start_frame = 0;
    state_reset_coins(state);
}

GameStateType state_get_type(const GameState* state) {
    if (!state) return GAME_STATE_START;
    return state->state;
}

void state_set_type(GameState* state, GameStateType type) {
    if (!state) return;
    state->state = type;
}

bool state_is_running(const GameState* state) {
    if (!state) return false;
    return state->running;
}

void state_set_running(GameState* state, bool running) {
    if (!state) return;
    state->running = running;
}

int state_get_frame_count(const GameState* state) {
    if (!state) return 0;
    return state->frame_count;
}

void state_set_frame_count(GameState* state, int frame_count) {
    if (!state) return;
    state->frame_count = frame_count;
}

void state_increment_frame_count(GameState* state) {
    if (!state) return;
    state->frame_count++;
}

int state_get_game_start_frame(const GameState* state) {
    if (!state) return 0;
    return state->game_start_frame;
}

void state_set_game_start_frame(GameState* state, int frame) {
    if (!state) return;
    state->game_start_frame = frame;
}

Player* state_get_player(GameState* state) {
    if (!state) return NULL;
    return state->player;
}

int state_get_current_map_id(const GameState* state) {
    if (!state) return 0;
    return state->current_map_id;
}

void state_set_current_map_id(GameState* state, int map_id) {
    if (!state) return;
    state->current_map_id = map_id;
}

int state_get_coins_collected(const GameState* state) {
    if (!state) return 0;
    return state->coins_collected;
}

void state_set_coins_collected(GameState* state, int count) {
    if (!state) return;
    state->coins_collected = count;
}

void state_increment_coins_collected(GameState* state) {
    if (!state) return;
    state->coins_collected++;
}

int state_get_total_coins(const GameState* state) {
    if (!state) return 0;
    return state->total_coins;
}

Map* state_get_maps(GameState* state) {
    if (!state) return NULL;
    return state->maps;
}

Map* state_get_current_map(GameState* state) {
    if (!state) return NULL;
    return &state->maps[state->current_map_id];
}

const HighScore* state_get_high_scores(const GameState* state, int* count) {
    if (!state) {
        if (count) *count = 0;
        return NULL;
    }
    if (count) *count = state->high_score_count;
    return state->high_scores;
}

HighScore* state_get_high_scores_mutable(GameState* state, int** count) {
    if (!state) {
        if (count) *count = NULL;
        return NULL;
    }
    if (count) *count = &state->high_score_count;
    return state->high_scores;
}

char* state_get_player_name(GameState* state) {
    if (!state) return NULL;
    return state->player_name;
}

int state_get_name_char_count(const GameState* state) {
    if (!state) return 0;
    return state->name_char_count;
}

void state_set_name_char_count(GameState* state, int count) {
    if (!state) return;
    state->name_char_count = count;
}

void state_increment_name_char_count(GameState* state) {
    if (!state) return;
    if (state->name_char_count < MAX_NAME_LENGTH) {
        state->name_char_count++;
    }
}

void state_decrement_name_char_count(GameState* state) {
    if (!state) return;
    if (state->name_char_count > 0) {
        state->name_char_count--;
    }
}

HighScore* state_get_pending_score(GameState* state) {
    if (!state) return NULL;
    return &state->pending_score;
}

bool state_all_coins_collected(const GameState* state) {
    if (!state) return false;
    
    int total_collected = 0;
    for (int i = 0; i < NUM_MAPS; i++) {
        for (int j = 0; j < state->maps[i].coin_count; j++) {
            if (state->maps[i].coins[j].collected) {
                total_collected++;
            }
        }
    }
    return total_collected >= state->total_coins;
}

void state_reset_coins(GameState* state) {
    if (!state) return;
    
    for (int i = 0; i < NUM_MAPS; i++) {
        for (int j = 0; j < state->maps[i].coin_count; j++) {
            state->maps[i].coins[j].collected = false;
        }
    }
}

