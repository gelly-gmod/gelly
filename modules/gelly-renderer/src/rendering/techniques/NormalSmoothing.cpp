#include "NormalSmoothing.h"

#include "detail/Shader.h"

const char *PIXEL_SHADER_SOURCE =
#include "generated/NormalEstimationPS.embed.hlsl"
	;

NormalSmoothing::NormalSmoothing(
	ID3D11Device *device, int depthWidth, int depthHeight, ID3D11Resource *depth
)
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

	// Create the depth SRV
	D3D11_TEX2D_SRV depthTexSRVDesc{};
	ZeroMemory(&depthTexSRVDesc, sizeof(depthTexSRVDesc));
	depthTexSRVDesc.MipLevels = 1;

	D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc{};
	ZeroMemory(&depthSRVDesc, sizeof(depthSRVDesc));
	depthSRVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthSRVDesc.Texture2D = depthTexSRVDesc;

	DX("Failed to create depth SRV",
	   device->CreateShaderResourceView(
		   depth, &depthSRVDesc, depthSRV.GetAddressOf()
	   ));

	// Create the depth sampler
	D3D11_SAMPLER_DESC depthSamplerDesc{};
	ZeroMemory(&depthSamplerDesc, sizeof(depthSamplerDesc));
	depthSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	depthSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	depthSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	depthSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	DX("Failed to create depth sampler",
	   device->CreateSamplerState(
		   &depthSamplerDesc, depthSampler.GetAddressOf()
	   ));
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
		};

		perFrameCBuffer.Set(context, &perFrameData);
	}

	// Clear the RTs
	float emptyColor[4] = {0.f, 0.f, 0.f, 0.f};
	context->ClearRenderTargetView(rts->normal.Get(), emptyColor);
	// Clear the depth buffer
	context->ClearDepthStencilView(
		rts->dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0
	);

	// Bind the RTs
	ID3D11RenderTargetView *renderTargets[1] = {rts->normal.Get()};
	context->OMSetRenderTargets(1, renderTargets, rts->dsv.Get());

	BindNDCQuad(context);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, depthSRV.GetAddressOf());
	context->PSSetSamplers(0, 1, depthSampler.GetAddressOf());

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
