#include <cstdio>
#include <raylib.h>
#include <GellyEngine.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

const int screenWidth = 1366;
const int screenHeight = 768;
const int fps = 60;
const char* title = "Gelly Engine Test Window";

void drawFluid(GellyScene* scene) {
    Camera3D camera = {0};
    camera.position = Vector3{0.0f, 10.0f, 10.0f};
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 15.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    BeginMode3D(camera);
    scene->EnterGPUWork();
    Vec4* positions = scene->GetPositions();
    for (int i = 0; i < scene->GetCurrentParticleCount(); i++) {
        DrawSphere(Vector3{positions[i].x, positions[i].y, positions[i].z}, 0.1f, BROWN);
    }

    if (IsKeyDown(KEY_SPACE)) {
        SetRandomSeed(GetRandomValue(-400, 400) + GetTime());
        scene->AddParticle(Vec4{0, 0, 0, 0.5f}, Vec3{static_cast<float>(GetRandomValue(-10, 10)) / 10.f, 0.1, static_cast<float>(GetRandomValue(-10, 10)) / 10.f});
    }
    scene->ExitGPUWork();
    scene->Update(GetFrameTime());

    EndMode3D();
}

void drawInfo(GellyScene* scene) {
    DrawText("gelly-engine test", 10, 10, 20, RAYWHITE);
    DrawText("press SPACE to add a particle", 10, 30, 20, RAYWHITE);
    char particleText[256];
    sprintf(particleText, "particles: %d", scene->GetCurrentParticleCount());
    DrawText(particleText, 10, 50, 20, RAYWHITE);
    DrawFPS(10, 70);
}

void drawGUI(GellyScene* scene) {
    if (GuiButton(Rectangle{10, 100, 100, 20}, "Shoot particles up")) {
        scene->EnterGPUWork();
        for (int i = 0; i < scene->GetCurrentParticleCount(); i++) {
            Vec3 oldVelocity = scene->GetVelocities()[i];
            scene->GetVelocities()[i] = Vec3{oldVelocity.x, 10, oldVelocity.z};
        }
        scene->ExitGPUWork();
    }
}
int main() {
    GellyScene* scene = GellyEngine_CreateScene(1000);
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(fps);


    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        drawFluid(scene);
        drawInfo(scene);
        drawGUI(scene);
        EndDrawing();
    }

    GellyEngine_DestroyScene(scene);
    CloseWindow();
    return 0;
}