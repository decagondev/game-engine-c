#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Game state structure
typedef struct {
    bool running;
    int frame_count;
    Vector2 position;  // Player position
    float speed;       // Movement speed
} GameState;

// Window settings
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_TITLE "Game Engine"
#define PLAYER_SPEED 5.0f
#define PLAYER_RADIUS 25.0f

// Initialize game
void game_init(GameState* game) {
    // Initialize raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60); // Set to run at 60 frames-per-second
    
    game->running = true;
    game->frame_count = 0;
    game->speed = PLAYER_SPEED;
    
    // Start player at center of screen
    game->position.x = SCREEN_WIDTH / 2.0f;
    game->position.y = SCREEN_HEIGHT / 2.0f;
    
    printf("Game initialized! Window created: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
}

// Update game logic
void game_update(GameState* game) {
    game->frame_count++;
    
    // Check for window close button or ESC key
    if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
        game->running = false;
    }
    
    // WASD movement input
    Vector2 movement = {0.0f, 0.0f};
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        movement.y -= game->speed;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        movement.y += game->speed;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        movement.x -= game->speed;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        movement.x += game->speed;
    }
    
    // Normalize diagonal movement to maintain consistent speed
    float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
    if (length > 0.0f) {
        movement.x = (movement.x / length) * game->speed;
        movement.y = (movement.y / length) * game->speed;
    }
    
    // Update position
    game->position.x += movement.x;
    game->position.y += movement.y;
    
    // Keep player within screen bounds
    if (game->position.x < PLAYER_RADIUS) {
        game->position.x = PLAYER_RADIUS;
    }
    if (game->position.x > SCREEN_WIDTH - PLAYER_RADIUS) {
        game->position.x = SCREEN_WIDTH - PLAYER_RADIUS;
    }
    if (game->position.y < PLAYER_RADIUS) {
        game->position.y = PLAYER_RADIUS;
    }
    if (game->position.y > SCREEN_HEIGHT - PLAYER_RADIUS) {
        game->position.y = SCREEN_HEIGHT - PLAYER_RADIUS;
    }
}

// Render game
void game_render(GameState* game) {
    BeginDrawing();
    
    // Clear background
    ClearBackground(RAYWHITE);
    
    // Draw UI information
    DrawText("WASD to move", 10, 10, 20, BLACK);
    DrawText(TextFormat("Position: (%.0f, %.0f)", game->position.x, game->position.y), 10, 35, 20, BLACK);
    DrawFPS(10, 60);
    
    // Draw player (circle)
    DrawCircleV(game->position, PLAYER_RADIUS, BLUE);
    DrawCircleV(game->position, PLAYER_RADIUS - 2, DARKBLUE);
    
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

