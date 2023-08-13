#ifndef GELLY_GELLYENGINE_H
#define GELLY_GELLYENGINE_H
#include "GellyScene.h"

GellyScene* GellyEngine_CreateScene(int maxParticles);
void GellyEngine_DestroyScene(GellyScene* scene);

#endif //GELLY_GELLYENGINE_H
