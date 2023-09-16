#include "OutputEncoder.h"

using namespace d3d11;

static const char *PIXEL_SHADER_SOURCE =
#include "shaders/d3d11/OutputEncoder.ps.embed.hlsl"
	;

OutputEncoder::OutputEncoder(ID3D11Device *device)
	: SSTechnique(device), pixelShader(nullptr) {
	ShaderCompileOptions options = {
		.device = device,
		.shader =
			{
				.buffer = (void *)PIXEL_SHADER_SOURCE,
				.size = strlen(PIXEL_SHADER_SOURCE),
				.name = "OutputEncoder.ps",
				.entryPoint = "main",
			},
		.defines = nullptr,
	};

	auto pixelShaderResult = compile_pixel_shader(options);
	pixelShader.Attach(pixelShaderResult.shader);
}

void OutputEncoder::RunForFrame(
	ID3D11DeviceContext *context, TechniqueResources *resources
) {
	GBuffer *gbuffer = resources->gbuffer;
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	gbuffer->output.depth.Clear(context, clearColor);
	gbuffer->output.normal.Clear(context, clearColor);

	Texture *outputTextures[] = {
		&gbuffer->output.depth,
		&gbuffer->output.normal,
	};

	d3d11::SetMRT(
		context, ARRAYSIZE(outputTextures), outputTextures, resources->dsv.Get()
	);

	context->PSSetShader(pixelShader.Get(), nullptr, 0);

	gbuffer->depth.SetAsSR(context, 0);
	gbuffer->normal.SetAsSR(context, 1);

	gbuffer->depth.SetSampler(context, 0);
	gbuffer->normal.SetSampler(context, 1);

	BindNDCQuad(context);

	context->Draw(4, 0);
	context->Flush();

	CleanupRTsAndShaders(context);
}