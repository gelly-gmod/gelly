#include "fluidrender/CD3D11RenderContext.h"

#include <GellyD3D.h>

#include <stdexcept>

#include "fluidrender/IRenderContext.h"

CD3D11RenderContext::CD3D11RenderContext(uint16_t width, uint16_t height)
	: device(nullptr), deviceContext(nullptr), width(width), height(height) {
	CreateDeviceAndContext();
}

void CD3D11RenderContext::CreateDeviceAndContext() {
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	DX("Failed to create D3D11 device",
	   D3D11CreateDevice(
		   nullptr,
		   D3D_DRIVER_TYPE_HARDWARE,
		   nullptr,
		   0,
		   nullptr,
		   0,
		   D3D11_SDK_VERSION,
		   &device,
		   &featureLevel,
		   &deviceContext
	   ));
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

void *CD3D11RenderContext::GetRenderAPIResource(RenderAPIResource resource) {
	switch (resource) {
		case RenderAPIResource::D3D11Device:
			return device;
		case RenderAPIResource::D3D11DeviceContext:
			return deviceContext;
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

	auto texture = new CD3D11ManagedTexture();
	texture->SetDesc(desc);
	texture->AttachToContext(this);
	texture->Create();
	textures[name] = texture;
	return texture;
}

void CD3D11RenderContext::DestroyTexture(const char *name) {
	auto texture = textures.find(name);
	if (texture == textures.end()) {
		throw std::logic_error("Texture does not exist");
	}

	texture->second->Destroy();
	delete texture->second;
	textures.erase(texture);
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
	deviceContext->Flush();
	
	// well-known query method to synchronize the GPU after
	// the commands finish executing

	D3D11_QUERY_DESC queryDesc = {};
	queryDesc.Query = D3D11_QUERY_EVENT;
	ID3D11Query *query;
	DX("Failed to create D3D11 query", device->CreateQuery(&queryDesc, &query));

	deviceContext->End(query);
	while (deviceContext->GetData(query, nullptr, 0, 0) == S_FALSE) {
		// spin
	}

	query->Release();

	HRESULT deviceRemoved = device->GetDeviceRemovedReason();

	if (deviceRemoved != S_OK) {
		DestroyAllTextures();
		CreateDeviceAndContext();
		CreateAllTextures();
	}
}

CD3D11RenderContext::~CD3D11RenderContext() {
	DestroyAllTextures();

	if (deviceContext) {
		deviceContext->Release();
		deviceContext = nullptr;
	}

	if (device) {
		device->Release();
		device = nullptr;
	}
}