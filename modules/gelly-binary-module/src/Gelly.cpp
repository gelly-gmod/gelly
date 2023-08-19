#include "Gelly.h"
#include <GMFS.h>

// TODO: MENTAL RECAP: THIS SHIT IS JUST EXITING. WHY? I DONT KNOW.
Gelly::Gelly(const GellyInitParams &params) :
    scene(GellyEngine_CreateScene(params.maxParticles, params.maxColliders)),
    renderer(GellyRenderer_Create({
        .maxParticles = params.maxParticles,
        .width = params.width,
        .height = params.height,
        .sharedTextures = params.sharedTextures
    })) {
    scene->RegisterD3DBuffer(renderer->GetD3DParticleBuffer(), params.maxParticles, sizeof(Vec4));
}

Gelly::~Gelly() {
    GellyRenderer_Destroy(renderer);
    GellyEngine_DestroyScene(scene);
}

void Gelly::LoadMap(const char *mapName) {
    char fullMapPath[256];
    sprintf_s(fullMapPath, "maps/%s.bsp", mapName);

    if (!FileSystem::Exists(fullMapPath)) {
        return;
    }

    FileHandle_t file = FileSystem::Open(fullMapPath, "rb");
    uint32_t size = FileSystem::Size(file);
    auto* buffer = static_cast<uint8_t *>(malloc(size));
    FileSystem::Read(buffer, FileSystem::Size(file), file);
    scene->AddBSP(mapName, buffer, size);
    free(buffer);
    FileSystem::Close(file);
}

void Gelly::Update(float deltaTime) {
    scene->EnterGPUWork();
    scene->colliders.Update();
    scene->ExitGPUWork();
    scene->Update(deltaTime);
}

void Gelly::Render() {
    renderer->Render();
};