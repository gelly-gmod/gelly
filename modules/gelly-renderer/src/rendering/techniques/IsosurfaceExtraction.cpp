#include "IsosurfaceExtraction.h"

static const char *PROGRAM_SOURCE =
#include "shaders/d3d11/IsosurfaceExtraction.cs.embed.hlsl"
	;

using namespace d3d11;

IsosurfaceExtraction::IsosurfaceExtraction(
	ID3D11Device *device, ID3D11Buffer *particleBuffer, int maxParticles
)
	: extractionProgram(
		  device, "IsosurfaceExtraction.cs.hlsl", "main", PROGRAM_SOURCE
	  ),
	  neighborBuffer(
		  device, maxParticles, nullptr, D3D11_BIND_UNORDERED_ACCESS
	  ),
	  remapBuffer(device, maxParticles, nullptr, D3D11_BIND_UNORDERED_ACCESS),
	  neighborCountBuffer(
		  device, maxParticles, nullptr, D3D11_BIND_UNORDERED_ACCESS
	  ),
	  positionSRV(device, particleBuffer, maxParticles),
	  neighborSRV(device, neighborBuffer),
	  remapSRV(device, remapBuffer),
	  neighborCountSRV(device, neighborCountBuffer),
	  layout({}) {
	layout.resources[0] = neighborSRV.Get();
	layout.resources[1] = neighborCountSRV.Get();
	layout.resources[2] = remapSRV.Get();
	layout.resources[3] = positionSRV.Get();
	layout.numResources = 4;
	layout.numViews = 0;
}

void IsosurfaceExtraction::RunForFrame(
	ID3D11DeviceContext *context, TechniqueResources *resources
) {
	// Update layout
	layout.views[0] = resources->gbuffer->normal.GetUAV();
	layout.resources[4] = resources->gbuffer->depth.GetSRV();
	layout.numResources = 5;

	// Isosurface tiles are 4x4.
	int groupX = static_cast<int>(
		ceilf(static_cast<float>(resources->gbuffer->width) / 4.f)
	);
	int groupY = static_cast<int>(
		ceilf(static_cast<float>(resources->gbuffer->height) / 4.f)
	);

	layout.numThreadsX = groupX;
	layout.numThreadsY = groupY;
	layout.constantBuffer = resources->perFrameCB;

	extractionProgram.Run(context, layout);

	CleanupRTsAndShaders(context, 1, 0);
}

[[nodiscard]] ID3D11Buffer *IsosurfaceExtraction::GetNeighborBuffer() const {
	return neighborBuffer.Get();
}

[[nodiscard]] ID3D11Buffer *IsosurfaceExtraction::GetRemapBuffer() const {
	return remapBuffer.Get();
}

[[nodiscard]] ID3D11Buffer *IsosurfaceExtraction::GetNeighborCountBuffer(
) const {
	return neighborCountBuffer.Get();
}