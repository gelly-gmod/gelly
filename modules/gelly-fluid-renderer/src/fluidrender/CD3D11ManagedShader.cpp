#include "fluidrender/CD3D11ManagedShader.h"

#include <d3d11.h>

#include <stdexcept>
#include <string>

CD3D11ManagedShader::CD3D11ManagedShader()
	: shader(std::monostate{}),
	  context(nullptr),
	  bytecode(nullptr),
	  bytecodeSize(0),
	  type(ShaderType::Vertex){};

CD3D11ManagedShader::~CD3D11ManagedShader() { CD3D11ManagedShader::Destroy(); }

bool CD3D11ManagedShader::IsEmpty() const { return shader.index() == 0; }

void CD3D11ManagedShader::AttachToContext(
	GellyObserverPtr<IRenderContext> context
) {
	this->context = context;
}

void CD3D11ManagedShader::SetBytecode(
	const uint8_t *bytecode, size_t bytecodeSize
) {
	this->bytecode = bytecode;
	this->bytecodeSize = bytecodeSize;
}

const uint8_t *CD3D11ManagedShader::GetBytecode() { return bytecode; }
size_t CD3D11ManagedShader::GetBytecodeSize() { return bytecodeSize; }

void CD3D11ManagedShader::SetType(ShaderType type) { this->type = type; }
ShaderType CD3D11ManagedShader::GetType() { return type; }

void CD3D11ManagedShader::Create() {
	if (!IsEmpty()) {
		Destroy();
	}

	auto *device = static_cast<ID3D11Device *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
	);

	switch (type) {
		case ShaderType::Vertex: {
			ID3D11VertexShader *vertexShader;
			if (const auto result = device->CreateVertexShader(
					bytecode, bytecodeSize, nullptr, &vertexShader
				);
				FAILED(result)) {
				throw std::runtime_error(
					"Failed to create vertex shader: " + std::to_string(result)
				);
			}
			shader = vertexShader;
			break;
		}
		case ShaderType::Pixel: {
			ID3D11PixelShader *pixelShader;
			if (const auto result = device->CreatePixelShader(
					bytecode, bytecodeSize, nullptr, &pixelShader
				);
				FAILED(result)) {
				throw std::runtime_error(
					"Failed to create pixel shader: " + std::to_string(result)
				);
			}
			break;
		}
	}
}

void CD3D11ManagedShader::Destroy() {
	if (!IsEmpty()) {
		switch (type) {
			case ShaderType::Vertex:
				const auto vertexShader =
					std::get<ID3D11VertexShader *>(shader);
				vertexShader->Release();
				break;
			case ShaderType::Pixel:
				const auto pixelShader = std::get<ID3D11PixelShader *>(shader);
				pixelShader->Release();
				break;
		}
		shader = std::monostate{};
	}

	// not really wrong, maybe later it'd be a good idea to
	// toss a logic error if a caller attempts to destroy
	// while the shader is empty
}

void CD3D11ManagedShader::Bind() {
	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
	);

	switch (type) {
		case ShaderType::Vertex:
			deviceContext->VSSetShader(
				std::get<ID3D11VertexShader *>(shader), nullptr, 0
			);
			break;
		case ShaderType::Pixel:
			deviceContext->PSSetShader(
				std::get<ID3D11PixelShader *>(shader), nullptr, 0
			);
			break;
	}
}