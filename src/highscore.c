#include "../include/highscore.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void highscore_load(HighScore* high_scores, int* count) {
    if (!high_scores || !count) return;
    
    *count = 0;
    
    FILE* file = fopen(HIGH_SCORE_FILENAME, "r");
    if (file == NULL) {
        return;
    }
    
    HighScore score;
    char line[256];
    
    while (*count < MAX_HIGH_SCORES && fgets(line, sizeof(line), file) != NULL) {
        char name_buffer[MAX_NAME_LENGTH + 1] = {0};
        if (sscanf(line, "%20s %d %d %f", name_buffer, &score.frame_count, 
                   &score.coins_collected, &score.health_remaining) == 4) {
            strncpy(score.name, name_buffer, MAX_NAME_LENGTH);
            score.name[MAX_NAME_LENGTH] = '\0';
            high_scores[*count] = score;
            (*count)++;
        }
    }
    
    fclose(file);
}

void highscore_save(const HighScore* high_scores, int count) {
    if (!high_scores) return;
    
    FILE* file = fopen(HIGH_SCORE_FILENAME, "w");
    if (file == NULL) {
        printf("Error: Could not save high scores to file\n");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %d %d %.1f\n", 
                high_scores[i].name,
                high_scores[i].frame_count,
                high_scores[i].coins_collected,
                high_scores[i].health_remaining);
    }
    
    fclose(file);
}

bool highscore_add(HighScore* high_scores, int* count, const char* name, 
                   int frame_count, int coins_collected, float health_remaining) {
    if (!high_scores || !count || !name) return false;
    
    HighScore new_score;
    strncpy(new_score.name, name, MAX_NAME_LENGTH);
    new_score.name[MAX_NAME_LENGTH] = '\0';
    new_score.frame_count = frame_count;
    new_score.coins_collected = coins_collected;
    new_score.health_remaining = health_remaining;
    
    int insert_pos = *count;
    for (int i = 0; i < *count; i++) {
        if (frame_count < high_scores[i].frame_count) {
            insert_pos = i;
            break;
        }
    }
    
    if (*count < MAX_HIGH_SCORES || insert_pos < MAX_HIGH_SCORES) {
        if (*count >= MAX_HIGH_SCORES) {
            *count = MAX_HIGH_SCORES - 1;
        }
        
        for (int i = *count; i > insert_pos; i--) {
            high_scores[i] = high_scores[i - 1];
        }
        
        high_scores[insert_pos] = new_score;
        (*count)++;
        
        highscore_save(high_scores, *count);
        printf("New high score added! Rank: %d, Name: %s, Frames: %d\n", 
               insert_pos + 1, name, frame_count);
        return true;
    }
    
    return false;
}

