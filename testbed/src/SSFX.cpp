#include "SSFX.h"

#include <memory>
#include <unordered_map>

#include "Rendering.h"
#include "Shaders.h"
#include "Textures.h"
#include "Window.h"
#include "wrl.h"

using namespace testbed;
using namespace Microsoft::WRL;

static ILogger *logger = nullptr;
static ID3D11Device *rendererDevice = nullptr;
static ID3D11DeviceContext *rendererContext = nullptr;

static ID3D11Buffer *quadVertexBuffer = nullptr;
static ID3D11InputLayout *quadInputLayout = nullptr;
static ID3D11VertexShader *quadVertexShader = nullptr;
static constexpr UINT VERTEX_SIZE = 6 * sizeof(float);
static constexpr UINT VERTEX_OFFSET = 0;

static constexpr const char *NDCQUAD_VERTEX_SHADER_PATH =
	"shaders/NDCQuad.vs50.hlsl.dxbc";

struct SSFXEffectResources {
	SSFXEffect effectData;
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11PixelShader> pixelShader;

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
		{"SV_POSITION",
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

void testbed::InitializeSSFXSystem(
	ILogger *newLogger, ID3D11Device *newRendererDevice
) {
	logger = newLogger;
	rendererDevice = newRendererDevice;
	rendererContext = GetRendererContext(rendererDevice);

	logger->Info("Creating NDC quad resources for SSFX...");
	CreateNDCQuadResources();
}

void testbed::RegisterSSFXEffect(const char *name, SSFXEffect effect) {
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

	resources->pixelShader.Attach(
		GetPixelShaderFromFile(rendererDevice, effect.pixelShaderPath)
	);

	effects[name] = std::move(resources);
}

SSFXEffect::ConstantDataPtr testbed::GetSSFXEffectConstantData(const char *name
) {
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

void testbed::UpdateSSFXEffectConstants(const char *name) {
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

void testbed::ApplySSFXEffect(const char *name) {
	const auto it = effects.find(name);
	if (it == effects.end()) {
		logger->Error("SSFX effect with name %s does not exist", name);
		return;
	}

	std::vector<ID3D11ShaderResourceView *> srvs(8, nullptr);
	std::vector<ID3D11RenderTargetView *> rtvs(8, nullptr);

	const auto effectPtr = it->second;
	for (const auto &inputTextureName : effectPtr->effectData.inputTextures) {
		srvs.push_back(GetTextureSRV(inputTextureName));
		rtvs.push_back(GetTextureRTV(inputTextureName));
	}

	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(WINDOW_WIDTH);
	viewport.Height = static_cast<float>(WINDOW_HEIGHT);
	viewport.MaxDepth = 1.f;
	viewport.MinDepth = 0.f;

	rendererContext->RSSetViewports(1, &viewport);

	rendererContext->OMSetRenderTargets(
		static_cast<UINT>(rtvs.size()), rtvs.data(), nullptr
	);

	rendererContext->PSSetShaderResources(
		0, static_cast<UINT>(srvs.size()), srvs.data()
	);

	rendererContext->VSSetShaderResources(
		0, static_cast<UINT>(srvs.size()), srvs.data()
	);

	rendererContext->IASetInputLayout(quadInputLayout);
	rendererContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);
	rendererContext->IASetVertexBuffers(
		0, 1, &quadVertexBuffer, &VERTEX_SIZE, &VERTEX_OFFSET
	);

	rendererContext->VSSetShader(quadVertexShader, nullptr, 0);
	rendererContext->PSSetShader(effectPtr->pixelShader.Get(), nullptr, 0);

	if (effectPtr->HasConstantBuffer()) {
		rendererContext->PSSetConstantBuffers(
			1, 1, effectPtr->constantBuffer.GetAddressOf()
		);

		rendererContext->VSSetConstantBuffers(
			1, 1, effectPtr->constantBuffer.GetAddressOf()
		);
	}

	rendererContext->Draw(4, 0);
	rendererContext->Flush();

	// Clear and reset for the next frame
	rendererContext->OMSetRenderTargets(0, nullptr, nullptr);
	rendererContext->VSSetShader(nullptr, nullptr, 0);
	rendererContext->PSSetShader(nullptr, nullptr, 0);
	rendererContext->VSSetConstantBuffers(0, 0, nullptr);
	rendererContext->PSSetConstantBuffers(0, 0, nullptr);
	rendererContext->VSSetShaderResources(0, 0, nullptr);
	rendererContext->PSSetShaderResources(0, 0, nullptr);
	rendererContext->IASetInputLayout(nullptr);
	rendererContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
	rendererContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
}
