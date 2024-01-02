#include "fluidrender/CD3D11RenderContext.h"

#include <GellyD3D.h>
#include <d3d11_4.h>

#include <stdexcept>

#include "fluidrender/CD3D11ManagedBuffer.h"
#include "fluidrender/CD3D11ManagedBufferLayout.h"
#include "fluidrender/CD3D11ManagedDepthBuffer.h"
#include "fluidrender/CD3D11ManagedShader.h"
#include "fluidrender/CD3D11MappedBufferView.h"
#include "fluidrender/CD3D11to11SharedTexture.h"
#include "fluidrender/CD3D9to11SharedTexture.h"
#include "fluidrender/IRenderContext.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

CD3D11RenderContext::CD3D11RenderContext(uint16_t width, uint16_t height)
	: device(nullptr),
	  deviceContext(nullptr),
	  width(width),
	  height(height),
	  textures({}),
	  shaders({}),
	  rasterizerState(nullptr) {
	CreateDeviceAndContext();
	fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
}

void CD3D11RenderContext::CreateDeviceAndContext() {
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
	UINT deviceFlags = 0;
#ifdef _DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	DX("Failed to create D3D11.1 device",
	   D3D11CreateDevice(
		   nullptr,
		   D3D_DRIVER_TYPE_HARDWARE,
		   nullptr,
		   deviceFlags,
		   nullptr,
		   0,
		   D3D11_SDK_VERSION,
		   &device,
		   &featureLevel,
		   &deviceContext
	   ));

#ifdef _DEBUG
	DX("Failed to get D3D11 debug interface",
	   device->QueryInterface(
		   __uuidof(ID3D11Debug), reinterpret_cast<void **>(&debug)
	   ));

	DX("Failed to get D3D11 info queue",
	   debug->QueryInterface(
		   __uuidof(ID3D11InfoQueue), reinterpret_cast<void **>(&infoQueue)
	   ));
#endif

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	DX("Failed to create D3D11 rasterizer state",
	   device->CreateRasterizerState(&rasterizerDesc, &rasterizerState));

	deviceContext->RSSetState(rasterizerState);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	// This is our thickness RT, we basically just wanna
	// accumulate the thickness of the particles
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;

	// And our absorption RT, we want to take the accumulated absorption
	// of the absorption vectors
	blendDesc.RenderTarget[1].BlendEnable = true;
	blendDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[1].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[1].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;

	DX("Failed to query for D3D11.4 (not supported?)",
	   device->QueryInterface(
		   __uuidof(ID3D11Device5), reinterpret_cast<void **>(&device5)
	   ));

	DX("Failed to query for D3D11.4 (DC)",
	   deviceContext->QueryInterface(
		   __uuidof(ID3D11DeviceContext4),
		   reinterpret_cast<void **>(&deviceContext4)
	   ));

	DX("Failed to create fence",
	   device5->CreateFence(
		   0,
		   D3D11_FENCE_FLAG_NONE,
		   __uuidof(ID3D11Fence),
		   reinterpret_cast<void **>(&frameCompletionFence)
	   ));

	DX("Failed to create D3D11 blend state",
	   device5->CreateBlendState(&blendDesc, &blendState));
}

void CD3D11RenderContext::CreateAllTextures() {
	for (auto &texture : textures) {
		if (!texture.second->Create()) {
			throw std::runtime_error(
				"Failed to create texture's underlying resource"
			);
		}
	}
}

void CD3D11RenderContext::DestroyAllTextures() {
	for (auto &texture : textures) {
		DestroyTexture(texture.first.c_str());
	}
}

void CD3D11RenderContext::CreateAllShaders() {
	for (const auto &shader : shaders) {
		shader->Create();
	}
}

void CD3D11RenderContext::DestroyAllShaders() {
	for (const auto &shader : shaders) {
		shader->Destroy();
	}
}

void *CD3D11RenderContext::GetRenderAPIResource(RenderAPIResource resource) {
	switch (resource) {
		case RenderAPIResource::D3D11Device:
			return device5;
		case RenderAPIResource::D3D11DeviceContext:
			return deviceContext4;
		default:
			return nullptr;
	}
}

ContextRenderAPI CD3D11RenderContext::GetRenderAPI() {
	return ContextRenderAPI::D3D11;
}

IManagedTexture *CD3D11RenderContext::CreateTexture(
	const char *name, const TextureDesc &desc
) {
	if (textures.find(name) != textures.end()) {
		throw std::logic_error("Texture already exists");
	}

	auto *texture = new CD3D11ManagedTexture();
	texture->SetDesc(desc);
	texture->AttachToContext(this);
	texture->Create();
	textures[name] = texture;
	return texture;
}

GellyObserverPtr<IManagedTexture> CD3D11RenderContext::CreateSharedTexture(
	const char *name, HANDLE sharedHandle, ContextRenderAPI guestAPI
) {
	if (textures.find(name) != textures.end()) {
		throw std::logic_error("Texture already exists");
	}

	IManagedTexture *texture = nullptr;
	switch (guestAPI) {
		case ContextRenderAPI::D3D11:
			texture = new CD3D11to11SharedTexture(sharedHandle);
			break;
		case ContextRenderAPI::D3D9Ex:
			texture = new CD3D9to11SharedTexture(sharedHandle);
			break;
		default:
			throw std::logic_error("Unsupported guest API");
	}

	texture->AttachToContext(this);
	texture->Create();
	textures[name] = texture;
	return texture;
}

GellyObserverPtr<IManagedShader> CD3D11RenderContext::CreateShader(
	const uint8_t *bytecode, size_t bytecodeSize, ShaderType type
) {
	auto *shader = new CD3D11ManagedShader();
	shader->AttachToContext(this);
	shader->SetBytecode(bytecode, bytecodeSize);
	shader->SetType(type);
	shader->Create();
	shaders.push_back(shader);
	return shader;
}

GellyObserverPtr<IManagedBuffer> CD3D11RenderContext::CreateBuffer(
	const BufferDesc &desc
) {
	auto *buffer = new CD3D11ManagedBuffer();
	buffer->SetDesc(desc);
	buffer->AttachToContext(this);
	buffer->Create();
	return buffer;
}

GellyInterfaceVal<IManagedBufferLayout> CD3D11RenderContext::CreateBufferLayout(
	const BufferLayoutDesc &desc
) {
	auto *layout = new CD3D11ManagedBufferLayout();
	layout->SetLayoutDesc(desc);
	layout->AttachToContext(this);
	layout->Create();
	return layout;
}

GellyOwnedInterface<IMappedBufferView>
CD3D11RenderContext::CreateMappedBufferView(
	GellyInterfaceRef<IManagedBuffer> buffer
) {
	auto view = std::make_unique<CD3D11MappedBufferView>();
	view->AttachToContext(this);
	view->View(buffer);

	// not at all required due to copy elision, but I like to be explicit
	return std::move(view);
}

GellyInterfaceVal<IManagedDepthBuffer> CD3D11RenderContext::CreateDepthBuffer(
	const DepthBufferDesc &desc
) {
	auto *depthBuffer = new CD3D11ManagedDepthBuffer();
	depthBuffer->SetDesc(desc);
	depthBuffer->AttachToContext(this);
	depthBuffer->Create();
	return depthBuffer;
}

void CD3D11RenderContext::DestroyTexture(const char *name) {
	const auto texture = textures.find(name);
	if (texture == textures.end()) {
		throw std::logic_error("Texture does not exist");
	}

	texture->second->Destroy();
	delete texture->second;
	textures.erase(texture);
}

void CD3D11RenderContext::BindMultipleTexturesAsOutput(
	GellyInterfaceVal<IManagedTexture> *textures,
	const uint8_t count,
	const IManagedTexture::OptionalDepthBuffer depthBuffer
) {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	ID3D11RenderTargetView *renderTargetViews[8] = {};
	ID3D11DepthStencilView *depthStencilView = nullptr;

	for (uint8_t i = 0; i < count; i++) {
		auto *texture = textures[i];
		if (texture == nullptr) {
			throw std::logic_error(
				"CD3D11RenderContext::BindMultipleTexturesAsOutput encountered "
				"a null texture"
			);
		}

		auto *rtv = texture->GetResource(TextureResource::D3D11_RTV);
		if (rtv == nullptr) {
			throw std::logic_error(
				"CD3D11RenderContext::BindMultipleTexturesAsOutput encountered "
				"a null RTV"
			);
		}

		renderTargetViews[i] = static_cast<ID3D11RenderTargetView *>(rtv);
	}

	if (depthBuffer.has_value()) {
		auto *dsv =
			depthBuffer.value()->RequestResource(DepthBufferResource::D3D11_DSV
			);

		if (dsv == nullptr) {
			throw std::logic_error(
				"CD3D11RenderContext::BindMultipleTexturesAsOutput encountered "
				"a null DSV"
			);
		}

		depthStencilView = static_cast<ID3D11DepthStencilView *>(dsv);
	}

	deviceContext4->OMSetRenderTargets(
		count, renderTargetViews, depthStencilView
	);
}

void CD3D11RenderContext::SetDimensions(uint16_t width, uint16_t height) {
	this->width = width;
	this->height = height;
}

void CD3D11RenderContext::GetDimensions(uint16_t &width, uint16_t &height) {
	width = this->width;
	height = this->height;
}

void CD3D11RenderContext::SubmitWork() {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	deviceContext4->Signal(frameCompletionFence, ++fenceValue);
	if (frameCompletionFence->GetCompletedValue() < fenceValue) {
		frameCompletionFence->SetEventOnCompletion(fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	if (const auto removeResult = device->GetDeviceRemovedReason();
		removeResult != S_OK) {
		DestroyAllTextures();
		DestroyAllShaders();
		ReleaseDevice();
		CreateDeviceAndContext();
		CreateAllTextures();
		CreateAllShaders();
	}
}

void CD3D11RenderContext::UseTextureResForNextDraw(
	GellyInterfaceRef<IManagedTexture> texture
) {
	const auto &desc = texture->GetDesc();
	overrideDimensions = true;
	overrideWidth = desc.width;
	overrideHeight = desc.height;
}

void CD3D11RenderContext::Draw(
	const uint32_t vertexCount,
	const uint32_t startVertex,
	const bool accumulate
) {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	uint16_t width = this->width;
	uint16_t height = this->height;

	if (overrideDimensions) {
		width = overrideWidth;
		height = overrideHeight;
		overrideDimensions = false;
	}

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	deviceContext4->RSSetViewports(1, &viewport);
	deviceContext4->RSSetState(rasterizerState);
	if (accumulate) {
		deviceContext4->OMSetBlendState(blendState, nullptr, 0xffffffff);
	}
	deviceContext4->Draw(vertexCount, startVertex);
}

void CD3D11RenderContext::ResetPipeline() {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	deviceContext4->OMSetRenderTargets(0, nullptr, nullptr);
	deviceContext4->ClearState();
}

void CD3D11RenderContext::SetRasterizerFlags(RasterizerFlags flags) {
	if (rasterizerFlags == flags) {
		return;
	}

	rasterizerFlags = flags;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	if ((flags & RasterizerFlags::DISABLE_CULL) != 0) {
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
	}

	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	rasterizerState->Release();

	DX("Failed to create D3D11 rasterizer state",
	   device5->CreateRasterizerState(&rasterizerDesc, &rasterizerState));
	deviceContext4->RSSetState(rasterizerState);
}

CD3D11RenderContext::~CD3D11RenderContext() {
	DestroyAllTextures();
	DestroyAllShaders();
	ReleaseDevice();

	CloseHandle(fenceEvent);

	if (blendState) {
		blendState->Release();
		blendState = nullptr;
	}

	if (rasterizerState) {
		rasterizerState->Release();
		rasterizerState = nullptr;
	}

	if (frameCompletionFence) {
		frameCompletionFence->Release();
		frameCompletionFence = nullptr;
	}
}

void CD3D11RenderContext::ReleaseDevice() {
	if (deviceContext) {
		deviceContext->Release();
		deviceContext = nullptr;
	}

	if (device) {
		device->Release();
		device = nullptr;
	}
}

#ifdef _DEBUG
void CD3D11RenderContext::PrintDebugInfo() {
	if (infoQueue == nullptr) {
		return;
	}

	const auto messageCount = infoQueue->GetNumStoredMessages();
	for (UINT i = 0; i < messageCount; i++) {
		SIZE_T messageLength;
		infoQueue->GetMessage(i, nullptr, &messageLength);

		auto *message = static_cast<D3D11_MESSAGE *>(malloc(messageLength));
		infoQueue->GetMessage(i, message, &messageLength);

		printf(
			"[CD3D11RenderContext::PrintDebugInfo]: %s\n", message->pDescription
		);

		free(message);
	}
}
#endif
