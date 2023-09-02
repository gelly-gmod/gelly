#include "GellyEngine.h"

#include <cstdio>

static NvFlexLibrary *library = nullptr;

static void flexErrorCallback(
	NvFlexErrorSeverity, const char *msg, const char *file, int line
) {
	printf("[gelly-engine]: %s - %s:%d\n", msg, file, line);
}

GellyScene *GellyEngine_CreateScene(
	int maxParticles, int maxColliders, void *d3d11Device
) {
	if (library == nullptr) {
		NvFlexInitDesc desc{};
		desc.enableExtensions = true;
		desc.computeType = eNvFlexD3D11;
		desc.renderDevice = d3d11Device;
		
		library = NvFlexInit(120, flexErrorCallback, &desc);
	}

	return new GellyScene(library, maxParticles, maxColliders);
}

void GellyEngine_DestroyScene(GellyScene *scene) { delete scene; }