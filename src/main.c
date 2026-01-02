#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>

// Game state structure
typedef struct {
    bool running;
    int frame_count;
} GameState;

// Window settings
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_TITLE "Game Engine"

// Initialize game
void game_init(GameState* game) {
    // Initialize raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60); // Set to run at 60 frames-per-second
    
    game->running = true;
    game->frame_count = 0;
    
    printf("Game initialized! Window created: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
}

// Update game logic
void game_update(GameState* game) {
    game->frame_count++;
    
    // Check for window close button or ESC key
    if (WindowShouldClose()) {
        game->running = false;
    }
    
    // Add your game logic here
}

// Render game
void game_render(GameState* game) {
    BeginDrawing();
    
    // Clear background
    ClearBackground(RAYWHITE);
    
    // Draw some basic content
    DrawText("Game Engine - Raylib Window", 190, 200, 20, DARKGRAY);
    DrawText(TextFormat("Frame: %d", game->frame_count), 10, 10, 20, BLACK);
    DrawFPS(10, 40);
    
    // Draw a simple shape to show it's working
    DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, BLUE);
    
    EndDrawing();
}

// Cleanup game resources
void game_cleanup(GameState* game) {
    CloseWindow(); // Close window and OpenGL context
    printf("Game cleanup. Total frames: %d\n", game->frame_count);
}

// Main game loop
int main(void) {
    GameState game;
    
    game_init(&game);
    
    // Main game loop
    while (game.running) {
        game_update(&game);
        game_render(&game);
    }
    
    game_cleanup(&game);
    
    return 0;
}

