#include "Pass.h"

const char *COMPOSITE_VS_SOURCE =
#include "shaders/d3d9/FullScreenQuad.vs.embed.hlsl"
	;

#define INIT_OPTIONS_FOR_SHADER(source, shaderName, shaderEntryPoint) \
	options.shader.buffer = (void *)source;                           \
	options.shader.size = strlen(source);                             \
	options.shader.name = shaderName;                                 \
	options.shader.entryPoint = shaderEntryPoint;

void Pass::CreateShaders(
	IDirect3DDevice9 *device, const char *pixelShaderSource
) {
	d3d9::ShaderCompileOptions options = {
		.device = device,
		.shader = {},
		.defines = nullptr,
	};

	INIT_OPTIONS_FOR_SHADER(COMPOSITE_VS_SOURCE, "FullScreenQuad.vs", "main");
	auto vertexShaderResult = d3d9::compile_vertex_shader(options);
	vertexShader.Attach(vertexShaderResult);

	INIT_OPTIONS_FOR_SHADER(pixelShaderSource, "FullScreenQuad.ps", "main");
	auto pixelShaderResult = d3d9::compile_pixel_shader(options);
}

void Pass::CreateScreenQuad(IDirect3DDevice9 *device) {
	device->CreateVertexBuffer(
		6 * sizeof(ScreenQuadVertex),
		0,
		ScreenQuadVertex::FVF,
		D3DPOOL_DEFAULT,
		screenQuad.GetAddressOf(),
		nullptr
	);

	ScreenQuadVertex *vertices;
	screenQuad->Lock(0, 0, reinterpret_cast<void **>(&vertices), 0);

	vertices[0] = {-1.f, -1.f, 0.f, 1.f, 0.f, 1.f};
	vertices[1] = {-1.f, 1.f, 0.f, 1.f, 0.f, 0.f};
	vertices[2] = {1.f, -1.f, 0.f, 1.f, 1.f, 1.f};
	vertices[3] = {1.f, -1.f, 0.f, 1.f, 1.f, 1.f};
	vertices[4] = {-1.f, 1.f, 0.f, 1.f, 0.f, 0.f};
	vertices[5] = {1.f, 1.f, 0.f, 1.f, 1.f, 0.f};

	screenQuad->Unlock();
}

void Pass::BindShadersAndQuad(IDirect3DDevice9 *device) {
	device->SetVertexShader(vertexShader.Get());
	device->SetPixelShader(pixelShader.Get());
	device->SetFVF(ScreenQuadVertex::FVF);
	device->SetStreamSource(0, screenQuad.Get(), 0, sizeof(ScreenQuadVertex));
}

Pass::Pass(IDirect3DDevice9 *device, const char *pixelShaderSource) {
	CreateShaders(device, pixelShaderSource);
	CreateScreenQuad(device);
}