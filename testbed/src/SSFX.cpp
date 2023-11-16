#include "SSFX.h"

#include <memory>
#include <unordered_map>

#include "Rendering.h"
#include "Shaders.h"
#include "Textures.h"
#include "wrl.h"

using namespace testbed;
using namespace Microsoft::WRL;

static ILogger *logger = nullptr;
static ID3D11Device *rendererDevice = nullptr;
static ID3D11DeviceContext *rendererContext = nullptr;

static ID3D11Buffer *quadVertexBuffer = nullptr;
static ID3D11InputLayout *quadInputLayout = nullptr;
static ID3D11VertexShader *quadVertexShader = nullptr;

constexpr const char *NDCQUAD_VERTEX_SHADER_PATH =
	"shaders/NDCQuad.vs50.hlsl.dxbc";

struct SSFXEffectResources {
	SSFXEffect effectData;
	ComPtr<ID3D11Buffer> constantBuffer;

	[[nodiscard]] inline bool HasConstantBuffer() const {
		return constantBuffer != nullptr;
	}
};

using SSFXEffectResourcesPtr = std::shared_ptr<SSFXEffectResources>;
static std::unordered_map<const char *, SSFXEffectResourcesPtr> effects;

static void CreateNDCQuadResources() {
	// Need to add vertex positions in NDC space along with
	// texture coordinates

	static const float quadVertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, /* bottom left */
		-1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f, /* top left */
		1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, /* bottom right */
		1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 1.0f  /* top right */
	};

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(quadVertices);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vertexBufferData{};
	vertexBufferData.pSysMem = quadVertices;

	auto hr = rendererDevice->CreateBuffer(
		&vertexBufferDesc, &vertexBufferData, &quadVertexBuffer
	);

	if (FAILED(hr)) {
		logger->Error("Failed to create NDC quad vertex buffer");
		return;
	}

	quadVertexShader =
		GetVertexShaderFromFile(rendererDevice, NDCQUAD_VERTEX_SHADER_PATH);

	const auto shaderBuffer =
		LoadShaderBytecodeFromFile(NDCQUAD_VERTEX_SHADER_PATH);

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{"POSITION",
		 0,
		 DXGI_FORMAT_R32G32B32A32_FLOAT,
		 0,
		 D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA,
		 0},
		{"TEXCOORD",
		 0,
		 DXGI_FORMAT_R32G32_FLOAT,
		 0,
		 D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA,
		 0}
	};

	hr = rendererDevice->CreateInputLayout(
		inputElementDescs,
		ARRAYSIZE(inputElementDescs),
		shaderBuffer.buffer,
		shaderBuffer.size,
		&quadInputLayout
	);

	FreeShaderBuffer(shaderBuffer);

	if (FAILED(hr)) {
		logger->Error("Failed to create NDC quad input layout");
		return;
	}
}

void InitializeSSFXSystem(ILogger *newLogger, ID3D11Device *newRendererDevice) {
	logger = newLogger;
	rendererDevice = newRendererDevice;
	rendererContext = GetRendererContext(rendererDevice);

	logger->Info("Creating NDC quad resources for SSFX...");
	CreateNDCQuadResources();
}

void RegisterSSFXEffect(const char *name, SSFXEffect effect) {
	if (effects.find(name) != effects.end()) {
		logger->Error("SSFX effect with name %s already exists", name);
		return;
	}

	SSFXEffectResourcesPtr resources = std::make_shared<SSFXEffectResources>();
	resources->effectData = effect;

	if (effect.shaderData) {
		D3D11_BUFFER_DESC constantBufferDesc{};
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.ByteWidth =
			static_cast<UINT>(effect.shaderData->size());
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		D3D11_SUBRESOURCE_DATA constantBufferData{};
		constantBufferData.pSysMem = effect.shaderData->data();

		HRESULT hr = rendererDevice->CreateBuffer(
			&constantBufferDesc, &constantBufferData, &resources->constantBuffer
		);

		if (FAILED(hr)) {
			logger->Error(
				"Failed to create constant buffer for SSFX effect %s", name
			);
			return;
		}
	}

	effects[name] = resources;
}

SSFXEffect::ConstantDataPtr GetSSFXEffectConstantData(const char *name) {
	const auto it = effects.find(name);
	if (it == effects.end()) {
		logger->Error("SSFX effect with name %s does not exist", name);
		return nullptr;
	}

	if (!it->second->HasConstantBuffer()) {
		logger->Error("SSFX effect %s does not have a constant buffer", name);
		return nullptr;
	}

	return it->second->effectData.shaderData;
}

void UpdateSSFXEffectConstants(const char *name) {
	const auto it = effects.find(name);
	if (it == effects.end()) {
		logger->Error("SSFX effect with name %s does not exist", name);
		return;
	}

	if (!it->second->HasConstantBuffer()) {
		logger->Error("SSFX effect %s does not have a constant buffer", name);
		return;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	HRESULT hr = rendererContext->Map(
		it->second->constantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);

	if (FAILED(hr)) {
		logger->Error("Failed to map constant buffer for SSFX effect %s", name);
		return;
	}

	std::memcpy(
		mappedResource.pData,
		it->second->effectData.shaderData->data(),
		it->second->effectData.shaderData->size()
	);

	rendererContext->Unmap(it->second->constantBuffer.Get(), 0);
}
