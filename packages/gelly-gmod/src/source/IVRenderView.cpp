#include "IVRenderView.h"

#include <MinHook.h>

#include <stdexcept>

#include "../logging/global-macros.h"
#include "Interface.h"

static IVRenderView *renderView = nullptr;

static void EnsureRenderView() {
	if (!renderView) {
		renderView = GetInterface<IVRenderView>(
			"engine.dll", VENGINE_RENDERVIEW_INTERFACE_VERSION
		);
	}
}

void GetMatricesFromView(
	const CViewSetup &view,
	VMatrix *pWorldToView,
	VMatrix *pViewToProjection,
	VMatrix *pWorldToProjection,
	VMatrix *pWorldToPixels
) {
	EnsureRenderView();
	renderView->GetMatricesForView(
		view,
		pWorldToView,
		pViewToProjection,
		pWorldToProjection,
		pWorldToPixels
	);
}

typedef void(__thiscall *DrawWorldLists_t)(
	IVRenderView *thisptr,
	void *_EDX,
	IWorldRenderList *list,
	unsigned long flags,
	float waterZAdjust
);

static DrawWorldLists_t originalDrawWorldLists = nullptr;
static DrawWorldLists_t hookedDrawWorldLists = nullptr;

static bool isWorldRendering = false;
static IDirect3DDevice9Ex *device = nullptr;
static IDirect3DSurface9 *depthStencilSurface = nullptr;

void __thiscall HookedDrawWorldLists(
	IVRenderView *thisptr,
	void *_EDX,
	IWorldRenderList *list,
	unsigned long flags,
	float waterZAdjust
) {
	isWorldRendering = true;
	device->SetDepthStencilSurface(depthStencilSurface);
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	originalDrawWorldLists(thisptr, _EDX, list, flags, waterZAdjust);
	isWorldRendering = false;
}

void SetupWorldRenderingHooks(
	IDirect3DDevice9Ex *device_, IDirect3DSurface9 *depthStencilSurface_
) {
	device = device_;
	depthStencilSurface = depthStencilSurface_;

	EnsureRenderView();
	void **vtable = *reinterpret_cast<void ***>(renderView);
	void *drawWorldLists = vtable[13];

	hookedDrawWorldLists = reinterpret_cast<DrawWorldLists_t>(drawWorldLists);

	if (MH_CreateHook(
			drawWorldLists,
			reinterpret_cast<void *>(&HookedDrawWorldLists),
			reinterpret_cast<LPVOID *>(&originalDrawWorldLists)
		) != MH_OK) {
		throw std::runtime_error(
			"Failed to create hook for IVRenderView::DrawWorldLists"
		);
	}

	// MH_EnableHook(drawWorldLists);
}

void TeardownWorldRenderingHooks() {
	MH_DisableHook(hookedDrawWorldLists);
	MH_RemoveHook(hookedDrawWorldLists);
}

bool IsWorldRendering() { return isWorldRendering; }
