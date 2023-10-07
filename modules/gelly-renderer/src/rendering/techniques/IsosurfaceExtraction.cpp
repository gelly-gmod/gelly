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
	  positionUAV(device, particleBuffer, maxParticles),
	  neighborUAV(device, neighborBuffer),
	  remapUAV(device, remapBuffer),
	  neighborCountUAV(device, neighborCountBuffer),
	  layout({}) {
	layout.views[0] = neighborUAV.Get();
	layout.views[1] = neighborCountUAV.Get();
	layout.views[2] = remapUAV.Get();
	layout.views[3] = positionUAV.Get();
	layout.numViews = 4;
}
