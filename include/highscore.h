#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <stdbool.h>

#define MAX_HIGH_SCORES 10
#define HIGH_SCORE_FILENAME "highscores.txt"
#define MAX_NAME_LENGTH 20

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int frame_count;
    int coins_collected;
    float health_remaining;
} HighScore;

/**
 * Load high scores from file.
 * @param high_scores Array to store high scores
 * @param count Output parameter for number of high scores loaded
 */
void highscore_load(HighScore* high_scores, int* count);

/**
 * Save high scores to file.
 * @param high_scores Array of high scores
 * @param count Number of high scores to save
 */
void highscore_save(const HighScore* high_scores, int count);

/**
 * Add a new high score if it qualifies.
 * @param high_scores Array of high scores (will be modified)
 * @param count Current count (will be updated)
 * @param name Player name
 * @param frame_count Completion frame count
 * @param coins_collected Number of coins collected
 * @param health_remaining Remaining health
 * @return true if high score was added, false otherwise
 */
bool highscore_add(HighScore* high_scores, int* count, const char* name, 
                   int frame_count, int coins_collected, float health_remaining);

#endif

