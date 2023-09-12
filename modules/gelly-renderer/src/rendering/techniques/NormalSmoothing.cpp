#include "NormalSmoothing.h"

#include <GellyD3D.h>

const char *PIXEL_SHADER_SOURCE =
#include "generated/NormalEstimationPS.embed.hlsl"
	;

using namespace d3d11;

NormalSmoothing::NormalSmoothing(ID3D11Device *device)
	: SSTechnique(device), perFrameCBuffer(device) {
	ShaderCompileOptions options = {
		.device = device,
		.shader =
			{
				.buffer = (void *)PIXEL_SHADER_SOURCE,
				.size = strlen(PIXEL_SHADER_SOURCE),
				.name = "NormalEstimationPS",
				.entryPoint = "main",
			},
		.defines = nullptr,
	};

	auto psCompile = compile_pixel_shader(options);
	pixelShader.Attach(psCompile.shader);
}

void NormalSmoothing::RunForFrame(
	ID3D11DeviceContext *context, TechniqueRTs *rts, const Camera &camera
) {
	// Upload the per-frame data
	{
		PerFrameCBuffer perFrameData = {
			.res = {rts->width, rts->height},
			.padding = {},
			.projection = camera.GetProjectionMatrix(),
			.view = camera.GetViewMatrix(),
			.invProj = camera.GetInvProjectionMatrix(),
			.invView = camera.GetInvViewMatrix(),
			.eye = camera.GetPosition(),
			.padding2 = {},
		};

		perFrameCBuffer.Set(context, &perFrameData);
	}

	// Clear the RTs
	float emptyColor[4] = {0.f, 0.f, 0.f, 0.f};
	rts->gbuffer->normal.Clear(context, emptyColor);

	// Clear the depth buffer
	context->ClearDepthStencilView(
		rts->dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0
	);

	// Bind the RTs
	rts->gbuffer->normal.SetAsRT(context, rts->dsv.Get());

	BindNDCQuad(context);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	rts->gbuffer->depth_low.SetAsSR(context, 0);
	rts->gbuffer->depth_low.SetSampler(context, 0);

	perFrameCBuffer.BindToShaders(context, 0);

	context->Draw(4, 0);
	context->Flush();

	// Unbind the shaders
	ID3D11ShaderResourceView *nullSRV[1] = {nullptr};
	context->PSSetShaderResources(0, 1, nullSRV);
	ID3D11SamplerState *nullSampler[1] = {nullptr};
	context->PSSetSamplers(0, 1, nullSampler);

	// Unbind the RTs
	ID3D11RenderTargetView *nullRT[1] = {nullptr};
	context->OMSetRenderTargets(1, nullRT, nullptr);
}
