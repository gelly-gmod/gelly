#include <cstdio>
#include <raylib.h>

const int screenWidth = 840;
const int screenHeight = 640;
const int fps = 60;
const char* title = "Gelly Engine Test Window";

int main() {
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("gelly-engine test", 10, 10, 20, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}