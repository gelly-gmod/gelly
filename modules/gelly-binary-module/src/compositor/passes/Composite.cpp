#include "Composite.h"

#include <GellyD3D.h>
#include <directxmath.h>
#include <source/IBaseClientDLL.h>
#include <source/IMaterialSystem.h>
#include <source/IVRenderView.h>

#include "TextureOverride.h"

const char *COMPOSITE_PS_SOURCE =
#include "shaders/d3d9/Composite.ps.embed.hlsl"
	;

Composite::Composite(IDirect3DDevice9 *device)
	: Pass(device, "Composite.ps", COMPOSITE_PS_SOURCE){};

void Composite::Render(PassResources *resources) {
	if (!TextureOverride_GetCubemapTexture()) {
		return;
	}

	auto *gbuffer = resources->gbuffer;
	auto gellyGBuffer = resources->gbuffer->shared;

	gellyGBuffer.depth->SetupAtStage(0, 0, resources->device);
	gellyGBuffer.normal->SetupAtStage(1, 1, resources->device);
	gbuffer->framebuffer.SetupAtStage(2, 2, resources->device);

	TextureOverride_ToggleCubemapFinding(false);
	auto *cubemap = TextureOverride_GetCubemapTexture();
	DX("Failed to set texture", resources->device->SetTexture(3, cubemap));
	TextureOverride_ToggleCubemapFinding(true);

	D3DMATRIX view{};
	D3DMATRIX projection{};
	VMatrix _unusedViewProj{};
	VMatrix _unusedViewProjWindow{};

	GetMatricesFromView(
		*resources->viewSetup,
		reinterpret_cast<VMatrix *>(&view),
		reinterpret_cast<VMatrix *>(&projection),
		&_unusedViewProj,
		&_unusedViewProjWindow
	);

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
	float origin[4] = {
		resources->viewSetup->origin.x,
		resources->viewSetup->origin.y,
		resources->viewSetup->origin.z,
		0};

	d3d9::SetConstantVectors(resources->device, 16, origin, 1);

	ExecutePass(resources->device);
}