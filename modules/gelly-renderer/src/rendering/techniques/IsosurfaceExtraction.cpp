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
		  device,
		  maxParticles * 64,
		  nullptr,
		  D3D11_BIND_SHADER_RESOURCE,
		  D3D11_USAGE_DEFAULT,
		  0,
		  D3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	  ),
	  internalToAPIBuffer(
		  device,
		  maxParticles,
		  nullptr,
		  D3D11_BIND_SHADER_RESOURCE,
		  D3D11_USAGE_DEFAULT,
		  0,
		  D3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	  ),
	  APIToInternalBuffer(
		  device,
		  maxParticles,
		  nullptr,
		  D3D11_BIND_SHADER_RESOURCE,
		  D3D11_USAGE_DEFAULT,
		  0,
		  D3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	  ),
	  neighborCountBuffer(
		  device,
		  maxParticles,
		  nullptr,
		  D3D11_BIND_SHADER_RESOURCE,
		  D3D11_USAGE_DEFAULT,
		  0,
		  D3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	  ),
	  positionSRV(
		  device,
		  DXGI_FORMAT_R32G32B32A32_FLOAT,
		  particleBuffer,
		  maxParticles,
		  false
	  ),
	  neighborSRV(device, DXGI_FORMAT_R32_UINT, neighborBuffer),
	  internalToAPISRV(device, DXGI_FORMAT_R32_UINT, internalToAPIBuffer),
	  APIToInternalSRV(device, DXGI_FORMAT_R32_UINT, APIToInternalBuffer),
	  neighborCountSRV(device, DXGI_FORMAT_R32_UINT, neighborCountBuffer),
	  layout({}) {
	layout.resources[0] = neighborSRV.Get();
	layout.resources[1] = neighborCountSRV.Get();
	layout.resources[2] = internalToAPISRV.Get();
	layout.resources[3] = APIToInternalSRV.Get();
	layout.resources[4] = positionSRV.Get();
	layout.numResources = 5;
	layout.numViews = 0;
}

void IsosurfaceExtraction::RunForFrame(
	ID3D11DeviceContext *context, TechniqueResources *resources
) {
	// Update layout
	layout.views[0] = resources->gbuffer->normal.GetUAV();
	layout.resources[5] = resources->gbuffer->depth.GetSRV();
	layout.numResources = 6;
	layout.numViews = 1;

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

	CleanupRTsAndShaders(context, 6, 0);
}

[[nodiscard]] ID3D11Buffer *IsosurfaceExtraction::GetNeighborBuffer() const {
	return neighborBuffer.Get();
}

[[nodiscard]] ID3D11Buffer *IsosurfaceExtraction::GetInternalToAPIBuffer(
) const {
	return internalToAPIBuffer.Get();
}

[[nodiscard]] ID3D11Buffer *IsosurfaceExtraction::GetAPIToInternalBuffer(
) const {
	return APIToInternalBuffer.Get();
}

[[nodiscard]] ID3D11Buffer *IsosurfaceExtraction::GetNeighborCountBuffer(
) const {
	return neighborCountBuffer.Get();
}