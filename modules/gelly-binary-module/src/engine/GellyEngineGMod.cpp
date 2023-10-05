#include "GellyEngineGMod.h"

#include <cassert>

GellyEngineGMod::GellyEngineGMod(
	int maxParticles, int maxColliders, void *d3d11Device
)
	: scene(nullptr) {
	scene = GellyEngine_CreateScene(maxParticles, maxColliders, d3d11Device);

	// This is really a completely unrecoverable error, and this should exit the
	// app while giving a reasonably detailed explanation to the user, but I
	// haven't standardized this across modules. For the time being, this should
	// do.
#ifdef _DEBUG
	assert(scene != nullptr);
#endif
}

GellyEngineGMod::~GellyEngineGMod() {
	if (!scene) {
		return;
	}

	GellyEngine_DestroyScene(scene);
}

GellyScene *GellyEngineGMod::GetScene() { return scene; }