#include "Composite.h"

#include <GellyD3D.h>

const char *COMPOSITE_PS_SOURCE =
#include "shaders/d3d9/Composite.ps.embed.hlsl"
	;

Composite::Composite(IDirect3DDevice9 *device)
	: Pass(device, "Composite.ps", COMPOSITE_PS_SOURCE){};

void Composite::Render(PassResources *resources) {
	auto *gbuffer = resources->gbuffer;
	auto gellyGBuffer = resources->gbuffer->shared;

	gellyGBuffer.depth->SetupAtStage(0, 0, resources->device);
	gbuffer->framebuffer.SetupAtStage(1, 0, resources->device);

	D3DMATRIX view;
	resources->device->GetTransform(D3DTS_VIEW, &view);
	D3DMATRIX projection;
	resources->device->GetTransform(D3DTS_PROJECTION, &projection);

	d3d9::SetConstantMatrix(resources->device, 0, view);
	d3d9::SetConstantMatrix(resources->device, 4, projection);

	ExecutePass(resources->device);
}