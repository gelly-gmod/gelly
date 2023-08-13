#ifndef GELLY_ENGINE_H
#define GELLY_ENGINE_H
#define DllExport __declspec( dllexport )
#include "Scene.h"

DllExport Scene* GellyEngine_CreateScene();
DllExport void GellyEngine_DestroyScene(Scene* scene);

#endif //GELLY_ENGINE_H
