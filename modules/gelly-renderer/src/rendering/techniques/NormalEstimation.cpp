#include "NormalEstimation.h"

static const char *PIXEL_SHADER_SOURCE =
#include "shaders/d3d11/NormalEstimation.ps.embed.hlsl"
	;

using namespace d3d11;

NormalEstimation::NormalEstimation(ID3D11Device *device) : SSTechnique(device) {
	ShaderCompileOptions options = {
		.device = device,
		.shader =
			{
				.buffer = (void *)PIXEL_SHADER_SOURCE,
				.size = strlen(PIXEL_SHADER_SOURCE),
				.name = "NormalEstimation.ps",
				.entryPoint = "main",
			},
		.defines = nullptr,
	};

	auto pixelShaderResult = compile_pixel_shader(options);
	pixelShader.Attach(pixelShaderResult.shader);
}

void NormalEstimation::RunForFrame(
	ID3D11DeviceContext *context, TechniqueResources *resources
) {
	GBuffer *gbuffer = resources->gbuffer;
	float clearColor[4] = {0.0f, 1.0f, 0.0f, 0.0f};
	gbuffer->normal.Clear(context, clearColor);

	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	gbuffer->filteredDepth.SetAsSR(context, 0);
	gbuffer->filteredDepth.SetSampler(context, 0);
	gbuffer->normal.SetAsRT(context, nullptr);
	resources->perFrameCB->BindToShaders(context, 0);

	BindNDCQuad(context);

	context->Draw(4, 0);
	context->Flush();

	CleanupRTsAndShaders(context, 1, 1);
}