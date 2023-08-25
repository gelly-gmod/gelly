#include "ParticleRendering.h"

#include <detail/Shader.h>

#include "detail/ErrorHandling.h"

// Technique overview:
// 1. Bind the particle vertex buffer to the input assembler.
// 2. Bind the particle vertex shader.
// 3. Bind the geometry shader and stream outputs.
// 4. Run the geometry shader.
// 5. Run the pipeline again on the new stream output.
// 6. Bind the pixel shader.
// 7. Run the pipeline.

static const char *PIXEL_SHADER_SOURCE =
#include "generated/FluidRendering_ParticleSplattingPS.embed.hlsl"
	;

static const char *VERTEX_SHADER_SOURCE =
#include "generated/FluidRendering_ParticleSplattingVS.embed.hlsl"
	;

static const char *GEOMETRY_SHADER_SOURCE =
#include "generated/FluidRendering_ParticleSplattingGS.embed.hlsl"
	;

#define INIT_OPTIONS_FOR_SHADER(source, shaderName, shaderEntryPoint) \
	options.shader.buffer = (void *)source;                           \
	options.shader.size = strlen(source);                             \
	options.shader.name = shaderName;                                 \
	options.shader.entryPoint = shaderEntryPoint;

void ParticleRendering::Initialize(ID3D11Device *device, int maxParticles) {
	ShaderCompileOptions options = {
		.device = device,
		.shader = {},
		.defines = nullptr,
	};

	INIT_OPTIONS_FOR_SHADER(
		PIXEL_SHADER_SOURCE, "FluidRendering_ParticleSplattingPS.hlsl", "main"
	);
	auto pixelShaderResult = compile_pixel_shader(options);
	pixelShader = pixelShaderResult.shader;

	INIT_OPTIONS_FOR_SHADER(
		VERTEX_SHADER_SOURCE, "FluidRendering_ParticleSplattingVS.hlsl", "main"
	);
	auto vertexShaderResult = compile_vertex_shader(options);
	vertexShader = vertexShaderResult.shader;
	vertexShaderBlob = vertexShaderResult.shaderBlob;

	INIT_OPTIONS_FOR_SHADER(
		GEOMETRY_SHADER_SOURCE,
		"FluidRendering_ParticleSplattingGS.hlsl",
		"main"
	);

	auto geometryShaderResult = compile_geometry_shader(options);

	geometryShader = geometryShaderResult.shader;

	// Create particle buffer
	D3D11_INPUT_ELEMENT_DESC inputLayout[1] = {};
	inputLayout[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
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
				(float)rand() / (float)RAND_MAX * 2.f,
				(float)rand() / (float)RAND_MAX * 12.f,
				(float)rand() / (float)RAND_MAX * 23.f,
				0.0f};
		}

		initData.pSysMem = initPositions;

		DX("Failed to make point instance buffer",
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

	this->maxParticles = maxParticles;
}

void ParticleRendering::RunForFrame(
	ID3D11DeviceContext *context, TechniqueRTs *rts, const Camera &camera
) {
	// Clear the RTs
	float emptyColor[4] = {0.f, 0.f, 0.f, 0.f};
	context->ClearRenderTargetView(rts->depth.Get(), emptyColor);

	// Bind the RTs
	// TODO: implement normals
	ID3D11RenderTargetView *renderTargets[1] = {rts->depth.Get()};
	context->OMSetRenderTargets(1, renderTargets, nullptr);

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

	context->Draw(maxParticles, 0);
	context->Flush();
}

ID3D11Buffer *ParticleRendering::GetParticleBuffer() const {
	return particleBuffer.Get();
}
