#include "DepthSmoothing.h"

static const char *PIXEL_SHADER_SOURCE =
#include "shaders/d3d11/DepthSmoothing.ps.embed.hlsl"
	;

using namespace d3d11;

DepthSmoothing::DepthSmoothing(ID3D11Device *device) : SSTechnique(device) {
	ShaderCompileOptions options = {
		.device = device,
		.shader =
			{
				.buffer = (void *)PIXEL_SHADER_SOURCE,
				.size = strlen(PIXEL_SHADER_SOURCE),
				.name = "DepthSmoothing.ps",
				.entryPoint = "main",
			},
		.defines = nullptr,
	};

	auto pixelShaderResult = compile_pixel_shader(options);
	pixelShader.Attach(pixelShaderResult.shader);
}

void DepthSmoothing::RunForFrame(
	ID3D11DeviceContext *context, TechniqueResources *resources
) {
	// THIS FUCKS EVERYTHING UP! WHY?
	GBuffer *gbuffer = resources->gbuffer;
	float clearColor[4] = {1.0f, 0.0f, 0.0f, 0.0f};

	gbuffer->filteredDepth.Clear(context, clearColor);

	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	gbuffer->filteredDepth.SetAsRT(context, nullptr);
	gbuffer->depth.SetAsSR(context, 0);
	gbuffer->depth.SetSampler(context, 0);
	resources->perFrameCB->BindToShaders(context, 0);

	BindNDCQuad(context);

	context->Draw(4, 0);
	context->Flush();

	CleanupRTsAndShaders(context, 0, 0);
}
