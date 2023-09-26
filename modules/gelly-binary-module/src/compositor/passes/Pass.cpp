#include "Pass.h"

const char *FULLSCREEN_VS_SOURCE =
#include "shaders/d3d9/FullScreenQuad.vs.embed.hlsl"
	;

#define INIT_OPTIONS_FOR_SHADER(source, shaderName, shaderEntryPoint) \
	options.shader.buffer = (void *)source;                           \
	options.shader.size = strlen(source);                             \
	options.shader.name = shaderName;                                 \
	options.shader.entryPoint = shaderEntryPoint;

void Pass::CreateShaders(
	IDirect3DDevice9 *device,
	const char *pixelShaderName,
	const char *pixelShaderSource
) {
	d3d9::ShaderCompileOptions options = {
		.device = device,
		.shader = {},
		.defines = nullptr,
	};

	INIT_OPTIONS_FOR_SHADER(FULLSCREEN_VS_SOURCE, "FullScreenQuad.vs", "main");
	auto vertexShaderResult = d3d9::compile_vertex_shader(options);
	vertexShader.Attach(vertexShaderResult);

	INIT_OPTIONS_FOR_SHADER(pixelShaderSource, pixelShaderName, "main");
	auto pixelShaderResult = d3d9::compile_pixel_shader(options);
	pixelShader.Attach(pixelShaderResult);
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

void Pass::ExecutePass(IDirect3DDevice9 *device) {
	BindShadersAndQuad(device);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
}

Pass::Pass(
	IDirect3DDevice9 *device,
	const char *pixelShaderName,
	const char *pixelShaderSource
) {
	CreateShaders(device, pixelShaderName, pixelShaderSource);
	CreateScreenQuad(device);
}
