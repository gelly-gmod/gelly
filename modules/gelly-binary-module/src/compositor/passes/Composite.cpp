#include "Composite.h"

const char *COMPOSITE_PS_SOURCE =
#include "shaders/d3d9/Composite.ps.embed.hlsl"
	;

Composite::Composite(IDirect3DDevice9 *device)
	: Pass(device, COMPOSITE_PS_SOURCE){};

void Composite::Render(PassResources *resources) {
	auto *gbuffer = resources->gbuffer;

	gbuffer->depth->SetupAtStage(0, 0, resources->device);
	gbuffer->normal->SetupAtStage(1, 0, resources->device);

	ExecutePass(resources->device);
}