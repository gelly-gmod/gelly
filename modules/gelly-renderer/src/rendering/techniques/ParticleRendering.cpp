#include "ParticleRendering.h"

#include <GellyD3D.h>

#include "detail/ErrorHandling.h"

static const char *PIXEL_SHADER_SOURCE =
#include "shaders/d3d11/ParticleRender.ps.embed.hlsl"
	;

static const char *VERTEX_SHADER_SOURCE =
#include "shaders/d3d11/ParticleRender.vs.embed.hlsl"
	;

static const char *GEOMETRY_SHADER_SOURCE =
#include "shaders/d3d11/ParticleRender.gs.embed.hlsl"
	;

using namespace d3d11;

#define INIT_OPTIONS_FOR_SHADER(source, shaderName, shaderEntryPoint) \
	options.shader.buffer = (void *)source;                           \
	options.shader.size = strlen(source);                             \
	options.shader.name = shaderName;                                 \
	options.shader.entryPoint = shaderEntryPoint;

ParticleRendering::ParticleRendering(ID3D11Device *device, int maxParticles)
	: perFrameCBuffer(device) {
	ShaderCompileOptions options = {
		.device = device,
		.shader = {},
		.defines = nullptr,
	};

	INIT_OPTIONS_FOR_SHADER(
		PIXEL_SHADER_SOURCE, "ParticleRender.ps.hlsl", "main"
	);
	auto pixelShaderResult = compile_pixel_shader(options);
	// .Attach has to be used to prevent releasing the underlying resource.
	pixelShader.Attach(pixelShaderResult.shader);

	INIT_OPTIONS_FOR_SHADER(
		VERTEX_SHADER_SOURCE, "ParticleRender.vs.hlsl", "main"
	);
	auto vertexShaderResult = compile_vertex_shader(options);
	vertexShader.Attach(vertexShaderResult.shader);
	vertexShaderBlob.Attach(vertexShaderResult.shaderBlob);

	INIT_OPTIONS_FOR_SHADER(
		GEOMETRY_SHADER_SOURCE, "ParticleRender.gs.hlsl", "main"
	);

	auto geometryShaderResult = compile_geometry_shader(options);
	geometryShader.Attach(geometryShaderResult.shader);

	// Create particle buffer
	D3D11_INPUT_ELEMENT_DESC inputLayout[1] = {};
	inputLayout[0] = {
		"SV_Position",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0};

	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(ParticlePoint) * maxParticles;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		auto *initPositions = new ParticlePoint[maxParticles];
		// Random particle data
		for (int i = 0; i < maxParticles; i++) {
			initPositions[i] = {
				rand() / (float)RAND_MAX * 4.f,
				rand() / (float)RAND_MAX * 4.f,
				rand() / (float)RAND_MAX * 4.f,
				1.f};
		}

		initData.pSysMem = initPositions;

		DX("Failed to make point buffer",
		   device->CreateBuffer(
			   &bufferDesc, &initData, particleBuffer.GetAddressOf()
		   ));

		delete[] initPositions;
	}

	DX("Failed to make particle input layout",
	   device->CreateInputLayout(
		   inputLayout,
		   1,
		   vertexShaderBlob->GetBufferPointer(),
		   vertexShaderBlob->GetBufferSize(),
		   particleInputLayoutBuffer.GetAddressOf()
	   ));
}

void ParticleRendering::RunForFrame(
	ID3D11DeviceContext *context, TechniqueResources *resources
) {
	// Upload the per-frame data
	perFrameCBuffer.Set(context, resources->perFrameCBData);

	// Clear the RTs
	GBuffer *gbuffer = resources->gbuffer;
	float emptyColor[4] = {0.f, 0.f, 0.f, 0.f};
	gbuffer->depth_low.Clear(context, emptyColor);
	gbuffer->depth_high.Clear(context, emptyColor);
	// Clear the depth buffer
	context->ClearDepthStencilView(
		resources->dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0
	);

	// Bind the RTs
	Texture *rtViews[2] = {&gbuffer->depth_low, &gbuffer->depth_high};
	d3d11::SetMRT(context, 2, rtViews, resources->dsv.Get());

	// Bind the particle buffer
	UINT stride = sizeof(ParticlePoint);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0, 1, particleBuffer.GetAddressOf(), &stride, &offset
	);

	context->IASetInputLayout(particleInputLayoutBuffer.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Bind the shaders
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->GSSetShader(geometryShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);

	perFrameCBuffer.BindToShaders(context, 0);

	context->Draw(activeParticles, 0);
	context->Flush();

	d3d11::CleanupRTsAndShaders(context);
}

ID3D11Buffer *ParticleRendering::GetParticleBuffer() const {
	return particleBuffer.Get();
}
