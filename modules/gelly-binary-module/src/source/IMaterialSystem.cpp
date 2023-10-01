#include "IMaterialSystem.h"

#include <cassert>

#include "Interface.h"

static IMaterialSystem *materialSystem = nullptr;

void EnsureMaterialSystem() {
	if (materialSystem == nullptr) {
		materialSystem = GetInterface<IMaterialSystem>(
			"materialsystem.dll", MATERIAL_SYSTEM_INTERFACE_VERSION
		);
	}

#ifdef _DEBUG
	assert(materialSystem != nullptr);
#endif
}

IMaterialSystem *GetMaterialSystem() {
	EnsureMaterialSystem();
	return materialSystem;
}

IMatRenderContext *GetRenderContext() {
	EnsureMaterialSystem();
	return GetMaterialSystem()->GetRenderContext();
}