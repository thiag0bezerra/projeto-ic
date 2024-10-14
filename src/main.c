#include "raylib.h"
#include "rcamera.h"
#include <stdlib.h>

#define MAX_COLUMNS 20
#define MAX_NPCS 5

typedef struct NPC {
    Vector3 position;
    float speed;
    int direction;  // 0 = move along X, 1 = move along Z
} NPC;

void UpdateNPCs(NPC* npcs, int count);

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Combined Game");

    // Define the camera (First-person)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int cameraMode = CAMERA_FIRST_PERSON;

    // Columns (Random objects)
    float heights[MAX_COLUMNS];
    Vector3 positions[MAX_COLUMNS];
    Color colors[MAX_COLUMNS];

    for (int i = 0; i < MAX_COLUMNS; i++) {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i] / 2.0f, (float)GetRandomValue(-15, 15) };
        colors[i] = (Color){ GetRandomValue(20, 255), GetRandomValue(10, 55), 30, 255 };
    }

    // NPCs (Random movement)
    NPC npcs[MAX_NPCS];
    for (int i = 0; i < MAX_NPCS; i++) {
        npcs[i].position = (Vector3){ (float)GetRandomValue(-15, 15), 1.0f, (float)GetRandomValue(-15, 15) };
        npcs[i].speed = (float)GetRandomValue(1, 3);
        npcs[i].direction = GetRandomValue(0, 1);
    }

    DisableCursor();
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update camera and NPCs
        UpdateCamera(&camera, cameraMode);
        UpdateNPCs(npcs, MAX_NPCS);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Ground

        // Draw columns
        for (int i = 0; i < MAX_COLUMNS; i++) {
            DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
            DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
        }

        // Draw NPCs
        for (int i = 0; i < MAX_NPCS; i++) {
            DrawCube(npcs[i].position, 1.0f, 1.0f, 1.0f, RED);
            DrawCubeWires(npcs[i].position, 1.0f, 1.0f, 1.0f, DARKGRAY);
        }

        EndMode3D();

        DrawText("Move: W, A, S, D", 10, 10, 20, BLACK);
        DrawText("NPCs move randomly", 10, 40, 20, BLACK);
        DrawFPS(10, 70);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}

void UpdateNPCs(NPC* npcs, int count)
{
    for (int i = 0; i < count; i++) {
        if (npcs[i].direction == 0) {
            npcs[i].position.x += npcs[i].speed * GetFrameTime();
            if (npcs[i].position.x > 15 || npcs[i].position.x < -15) npcs[i].speed *= -1;
        } else {
            npcs[i].position.z += npcs[i].speed * GetFrameTime();
            if (npcs[i].position.z > 15 || npcs[i].position.z < -15) npcs[i].speed *= -1;
        }
    }
}
