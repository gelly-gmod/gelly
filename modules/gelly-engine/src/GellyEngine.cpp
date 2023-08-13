#include <cstdio>
#include "GellyEngine.h"

static NvFlexLibrary* library = nullptr;

static void flexErrorCallback(NvFlexErrorSeverity, const char* msg, const char* file, int line) {
    printf("[gelly-engine]: %s - %s:%d\n", msg, file, line);
}

GellyScene* GellyEngine_CreateScene(int maxParticles) {
    if (library == nullptr) {
        NvFlexInitDesc desc{};
        desc.enableExtensions = true;
        desc.computeType = eNvFlexCUDA;
        library = NvFlexInit(120, flexErrorCallback, &desc);
    }

    return new GellyScene(library, maxParticles);
}

void GellyEngine_DestroyScene(GellyScene* scene) {
    delete scene;
}