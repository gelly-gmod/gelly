#include "SSTechnique.h"

#include <GellyD3D.h>

#include "detail/ErrorHandling.h"

const char *VERTEX_SHADER_SOURCE =
#include "shaders/d3d11/NDCQuad.vs.embed.hlsl"
	;

using namespace d3d11;

static const float screenQuadVerts[] = {
	-1.0f, -1.0f, 0.0f, 1.0f,  // xyzw
	0.0f,  1.0f,			   // uv
	-1.0f, 1.0f,  0.0f, 1.0f,  // xyzw
	0.0f,  0.0f,			   // uv
	1.0f,  -1.0f, 0.0f, 1.0f,  // xyzw
	1.0f,  1.0f,			   // uv
	1.0f,  1.0f,  0.0f, 1.0f,  // xyzw
	1.0f,  0.0f,			   // uv
};

SSTechnique::SSTechnique(ID3D11Device *device)
	: vertexBuffer(
		  device,
		  (Vertex *)screenQuadVerts,
		  4,
		  D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	  ) {
	// Compile the NDC quad vertex shader
	ShaderCompileOptions options = {
		.device = device,
		.shader =
			{
				.buffer = (void *)VERTEX_SHADER_SOURCE,
				.size = strlen(VERTEX_SHADER_SOURCE),
				.name = "NDCQuad.vs",
				.entryPoint = "main",
			},
		.defines = nullptr,
	};

	auto vsCompile = compile_vertex_shader(options);
	vertexShader.Attach(vsCompile.shader);
	vertexShaderBlob.Attach(vsCompile.shaderBlob);

	// Create the input layout
	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
		{
			.SemanticName = "SV_Position",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
	};

	DX("Failed to create input layout",
	   device->CreateInputLayout(
		   inputLayoutDesc,
		   ARRAYSIZE(inputLayoutDesc),
		   vertexShaderBlob->GetBufferPointer(),
		   vertexShaderBlob->GetBufferSize(),
		   inputLayout.GetAddressOf()
	   ));
}

void SSTechnique::BindNDCQuad(ID3D11DeviceContext *context) {
	vertexBuffer.SetAtSlot(context, 0, inputLayout.Get());
}
