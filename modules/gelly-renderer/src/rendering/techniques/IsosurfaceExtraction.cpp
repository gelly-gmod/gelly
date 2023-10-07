#include "IsosurfaceExtraction.h"

static const char *PROGRAM_SOURCE =
#include "shaders/d3d11/IsosurfaceExtraction.cs.embed.hlsl"
	;

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
