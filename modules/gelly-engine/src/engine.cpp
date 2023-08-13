#include "engine.h"

Scene* GellyEngine_CreateScene() {
    return new Scene();
}

void GellyEngine_DestroyScene(Scene* scene) {
    delete scene;
}