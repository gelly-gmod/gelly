#include "Composite.h"

#include <DirectXMath.h>
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
	gbuffer->framebuffer.SetupAtStage(1, 1, resources->device);

	D3DMATRIX view;
	resources->device->GetTransform(D3DTS_VIEW, &view);
	D3DMATRIX projection;
	resources->device->GetTransform(D3DTS_PROJECTION, &projection);

	d3d9::SetConstantMatrix(resources->device, 0, view);
	d3d9::SetConstantMatrix(resources->device, 4, projection);

	XMFLOAT4X4 invView{};
	XMFLOAT4X4 convertedView{};
	d3d9::ConvertMatrix(view, &convertedView);
	XMStoreFloat4x4(
		&invView, XMMatrixInverse(nullptr, XMLoadFloat4x4(&convertedView))
	);

	XMFLOAT4X4 invProjection{};
	XMFLOAT4X4 convertedProjection{};
	d3d9::ConvertMatrix(projection, &convertedProjection);

	XMStoreFloat4x4(
		&invProjection,
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&convertedProjection))
	);

	D3DMATRIX convertedInvView{};
	d3d9::ConvertMatrix(invView, &convertedInvView);

	D3DMATRIX convertedInvProjection{};
	d3d9::ConvertMatrix(invProjection, &convertedInvProjection);

	d3d9::SetConstantMatrix(resources->device, 8, convertedInvView);
	d3d9::SetConstantMatrix(resources->device, 12, convertedInvProjection);

	ExecutePass(resources->device);
}