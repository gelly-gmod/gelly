#include "IsosurfaceExtraction.h"

static const char *PROGRAM_SOURCE =
#include "shaders/d3d11/IsosurfaceExtraction.ps.embed.hlsl"
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
	  layout({}) {}