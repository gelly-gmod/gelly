#include "fluidrender/CD3D11DebugFluidRenderer.h"

#include <d3d11.h>

CD3D11DebugFluidRenderer::CD3D11DebugFluidRenderer()
	: context(nullptr), simData(nullptr) {}

void CD3D11DebugFluidRenderer::SetSimData(GellyObserverPtr<ISimData> simData) {
	this->simData = simData;
}