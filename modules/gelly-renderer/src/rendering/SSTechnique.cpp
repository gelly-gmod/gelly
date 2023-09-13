#include "SSTechnique.h"

#include <GellyD3D.h>

#include "detail/ErrorHandling.h"

const char *VERTEX_SHADER_SOURCE =
#include "shaders/d3d11/NDCQuad.vs.embed.hlsl"
	;

using namespace d3d11;

SSTechnique::SSTechnique(ID3D11Device *device) {
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

	// Create the vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc{};
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(float) * 6 * 4;	 // xyzw, uv
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f,  // xyzw
		0.0f,  1.0f,			   // uv
		-1.0f, 1.0f,  0.0f, 1.0f,  // xyzw
		0.0f,  0.0f,			   // uv
		1.0f,  -1.0f, 0.0f, 1.0f,  // xyzw
		1.0f,  1.0f,			   // uv
		1.0f,  1.0f,  0.0f, 1.0f,  // xyzw
		1.0f,  0.0f,			   // uv
	};

	D3D11_SUBRESOURCE_DATA vertexBufferData{};
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = vertices;

	DX("Failed to create vertex buffer",
	   device->CreateBuffer(
		   &vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf()
	   ));
}

void SSTechnique::BindNDCQuad(ID3D11DeviceContext *context) {
	UINT stride = sizeof(float) * 6;
	UINT offset = 0;
	context->IASetVertexBuffers(
		0, 1, vertexBuffer.GetAddressOf(), &stride, &offset
	);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(inputLayout.Get());
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
}
