#include "IRenderView.h"

#include <stdexcept>

const char *GELLY_RENDERVIEW_INTERFACE_VERSION = "VEngineRenderView014";
const int RENDERVIEW_SCENEBEGIN_VTABLE_INDEX = 13;

static void copy_renderview_vtable(void **vtable, int vtableCount) {
	auto *renderview = GetInterface<IVRenderView>(
		"engine.dll", GELLY_RENDERVIEW_INTERFACE_VERSION
	);

	auto **renderview_vtable = *reinterpret_cast<void ***>(renderview);

	memcpy(vtable, renderview_vtable, vtableCount * sizeof(void *));
}

void HookSceneBegin(
	IVRenderView_SceneBeginFn newFn, IVRenderView_SceneBeginFn *oldFn
) {
	void **vtable[15];	// Just enough to hold the vtable
	copy_renderview_vtable(reinterpret_cast<void **>(vtable), 15);

	if (MH_CreateHook(
			vtable[RENDERVIEW_SCENEBEGIN_VTABLE_INDEX],
			reinterpret_cast<void *>(newFn),
			reinterpret_cast<void **>(oldFn)
		) != MH_OK) {
		throw std::runtime_error("Failed to hook IVRenderView::SceneBegin");
	}

	if (MH_EnableHook(vtable[RENDERVIEW_SCENEBEGIN_VTABLE_INDEX]) != MH_OK) {
		throw std::runtime_error("Failed to enable IVRenderView::SceneBegin");
	}
}
