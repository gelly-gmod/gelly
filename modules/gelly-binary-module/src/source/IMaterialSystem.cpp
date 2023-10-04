#include "IMaterialSystem.h"

#include <cassert>

#include "Interface.h"
#include "hooking/Library.h"

static IMaterialSystem *materialSystem = nullptr;
static Library materialSystemLib;

// Particularly long because this function is pretty simple so we don't want to
// get it confused with something else.
static const char *GetLocalCubemapSignature =
	"40 53 48 83 ec 20 48 8b d9 48 8d 0d e4 3d 0c 00 ff 15 82 c7 08 00 48 8b "
	"c8 48 85 c0 75 07 48 8d 8b f0 24 00 00";

typedef CTexture *(__thiscall *GetLocalCubemapFn)(void *);

static GetLocalCubemapFn getLocalCubemap = nullptr;

void EnsureMaterialSystem() {
	if (materialSystem == nullptr) {
		materialSystem = GetInterface<IMaterialSystem>(
			"materialsystem.dll", MATERIAL_SYSTEM_INTERFACE_VERSION
		);

		materialSystemLib.Init("materialsystem.dll");
		getLocalCubemap = materialSystemLib.FindFunction<GetLocalCubemapFn>(
			GetLocalCubemapSignature
		);
	}

#ifdef _DEBUG
	assert(materialSystem != nullptr);
	assert(getLocalCubemap != nullptr);
#endif
}

CTexture *GetLocalCubemap() {
	EnsureMaterialSystem();
	return getLocalCubemap(materialSystem);
}

IMaterialSystem *GetMaterialSystem() {
	EnsureMaterialSystem();
	return materialSystem;
}

IMatRenderContext *GetRenderContext() {
	EnsureMaterialSystem();
	return GetMaterialSystem()->GetRenderContext();
}