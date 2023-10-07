#include "IsosurfaceExtraction.h"

IsosurfaceExtraction::IsosurfaceExtraction(
	ID3D11Device *device, int maxParticles
)
	: neighborBuffer(
		  device, maxParticles, nullptr, D3D11_BIND_UNORDERED_ACCESS
	  ),
	  remapBuffer(device, maxParticles, nullptr, D3D11_BIND_UNORDERED_ACCESS),
	  neighborCountBuffer(
		  device, maxParticles, nullptr, D3D11_BIND_UNORDERED_ACCESS
	  ),
	  neighborUAV(device, neighborBuffer),
	  remapUAV(device, remapBuffer),
	  neighborCountUAV(device, neighborCountBuffer),
	  layout({}) {}