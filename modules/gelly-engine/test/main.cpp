#include <cstdio>
#include <raylib.h>
#include <GellyEngine.h>
#include <vector>

#define RAYGUI_IMPLEMENTATION

#include <raygui.h>

const int screenWidth = 1366;
const int screenHeight = 768;
const int fps = 60;
const char *title = "Gelly Engine Test Window";
static std::vector<Model> models;

void drawFluid(GellyScene *scene) {
    Camera3D camera = {0};
    camera.position = Vector3{0.0f, 10.0f, 10.0f};
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 15.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    BeginMode3D(camera);
    scene->EnterGPUWork();
    scene->colliders.Update();

    for (auto &model : models) {
        DrawModelWires(model, Vector3{0, 0, 0}, 1.0f, WHITE);
        DrawBoundingBox(GetModelBoundingBox(model), RED);
    }

    Vec4 *positions = scene->GetPositions();
    for (int i = 0; i < scene->GetCurrentParticleCount(); i++) {
        Vector2 screenPosition = GetWorldToScreen(Vector3{positions[i].x, positions[i].y, positions[i].z}, camera);
        if (screenPosition.x < 0 || screenPosition.y < 0 || screenPosition.x > screenWidth ||
            screenPosition.y > screenHeight) {
            continue;
        }
        DrawSphere(Vector3{positions[i].x, positions[i].y, positions[i].z}, 0.1f, BROWN);
    }

    if (IsKeyDown(KEY_SPACE)) {
        SetRandomSeed(GetRandomValue(-400, 400) + GetTime());
        scene->AddParticle(Vec4{0, 0, 0, 0.5f}, Vec3{static_cast<float>(GetRandomValue(-10, 10)) / 10.f, 0.1,
                                                     static_cast<float>(GetRandomValue(-10, 10)) / 10.f});
    }
    scene->ExitGPUWork();
    scene->Update(GetFrameTime());

    EndMode3D();
}

void drawInfo(GellyScene *scene) {
    DrawText("gelly-engine test simulation", 10, 10, 20, RAYWHITE);
    DrawText("press SPACE to add a particle", 10, 30, 20, RAYWHITE);
    char particleText[256];
    sprintf(particleText, "particles: %d", scene->GetCurrentParticleCount());
    DrawText(particleText, 10, 50, 20, RAYWHITE);
    DrawFPS(10, 70);
}

static float GUI_radiusValue = 0.f;
static float GUI_collisionDistanceValue = 0.f;
static float GUI_surfaceTensionValue = 0.f;
static float GUI_viscosityValue = 0.f;
#define PARAM_SLIDER(index, name, leftText, rightText, minValue, maxValue) \
    GuiSlider(Rectangle{10, 160 + index * 30, 300, 20}, leftText, rightText, &GUI_##name##Value, minValue, maxValue); \
    scene->params->name = GUI_##name##Value;
void drawGUI(GellyScene *scene) {
    if (GuiButton(Rectangle{10, 100, 100, 20}, "Shoot particles up")) {
        scene->EnterGPUWork();
        for (int i = 0; i < scene->GetCurrentParticleCount(); i++) {
            Vec3 oldVelocity = scene->GetVelocities()[i];
            scene->GetVelocities()[i] = Vec3{oldVelocity.x, 10, oldVelocity.z};
        }
        scene->ExitGPUWork();
    }

    if (GuiButton(Rectangle{10, 130, 100, 20}, "Add test mesh")) {
        scene->colliders.EnterGPUWork();
        Mesh plane = GenMeshPlane(1, 1, 2, 2);
        MeshUploadInfo info{};
        info.vertices = reinterpret_cast<Vec3 *>(plane.vertices);
        info.vertexCount = plane.vertexCount;
        int* indices = static_cast<int *>(malloc(sizeof(int) * plane.vertexCount));
        for (int i = 0; i < plane.vertexCount; i++) {
            indices[i] = i;
        }

        info.indices = indices;
        info.indexCount = plane.vertexCount;
        BoundingBox box = GetMeshBoundingBox(plane);
        info.lower = Vec3{-1000.f, -1000.f, -1000.f};
        info.upper = Vec3{1000.f, 1000.f, 1000.f};

        auto id = scene->colliders.AddTriangleMesh(info);
        GellyEntity entity{};
        entity.position = Vec3{0, 0, 0};
        entity.rotation = Quat{0, 0, 0, 1};
        entity.mesh = id;
        scene->colliders.AddEntity(entity);
        scene->colliders.ExitGPUWork();

        free(indices);

        models.push_back(LoadModelFromMesh(plane));
    }

    PARAM_SLIDER(0, radius, "0.1 radius", "10 radius", 0.2f, 10.f);
    PARAM_SLIDER(1, collisionDistance, "0.1 collision distance", "10 collision distance", 0.1f, 10.f);
    PARAM_SLIDER(2, surfaceTension, "0.1 surface tension", "10 surface tension", 0.1f, 10.f);
    PARAM_SLIDER(3, viscosity, "0.1 viscosity", "10 viscosity", 0.1f, 10.f);
}

int main() {
    GellyScene *scene = GellyEngine_CreateScene(1000, 100);
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