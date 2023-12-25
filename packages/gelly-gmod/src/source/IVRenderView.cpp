#include "IVRenderView.h"
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