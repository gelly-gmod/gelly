
#include "Textures.h"

#include <wrl.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <tracy/Tracy.hpp>
#include <unordered_map>

#include "Rendering.h"
#include "d3d11.h"

using namespace testbed;
using namespace Microsoft::WRL;

template <typename T>
using UnownedResource = T *;

template <template <typename> class PtrType>
struct TextureEntry {
	PtrType<ID3D11ShaderResourceView> srv;
	PtrType<ID3D11RenderTargetView> rtv;
	PtrType<ID3D11Texture2D> texture;
	PtrType<ID3D11SamplerState> sampler;
};

using OwnedTexturePtr = std::shared_ptr<TextureEntry<ComPtr>>;
using UnownedTexturePtr = std::shared_ptr<TextureEntry<UnownedResource>>;

static std::unordered_map<std::string, OwnedTexturePtr> textures;
static std::unordered_map<std::string, UnownedTexturePtr> unownedTextures;

static ILogger *logger = nullptr;
static ID3D11Device *rendererDevice = nullptr;

bool OwnedTextureExists(const char *name) {
	return textures.find(name) != textures.end();
}

bool UnownedTextureExists(const char *name) {
	return unownedTextures.find(name) != unownedTextures.end();
}

OwnedTexturePtr GetOwnedTexture(const char *name) {
	const auto it = textures.find(name);
	if (it == textures.end()) {
		logger->Error("Texture with name %s does not exist", name);
		return nullptr;
	}

	// invokes copy constructor
	return it->second;
}

UnownedTexturePtr GetUnownedTexture(const char *name) {
	const auto it = unownedTextures.find(name);
	if (it == unownedTextures.end()) {
		logger->Error("Unowned texture with name %s does not exist", name);
		return nullptr;
	}

	// invokes copy constructor
	return it->second;
}

void testbed::InitializeTextureSystem(
	ILogger *newLogger, ID3D11Device *rendererDevice
) {
	logger = newLogger;
	::rendererDevice = rendererDevice;

	logger->Info("Initialized texture system");

	// So, the main renderer already creates an RTV and a texture for
	// the back buffer, so we don't need to do that here.
	// However we do need to create an SRV for the back buffer

	UnownedTextureInfo backBufferInfo{};
	backBufferInfo.texture = GetBackBuffer(rendererDevice);
	backBufferInfo.rtv = GetBackBufferRTV(rendererDevice);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	auto result = rendererDevice->CreateShaderResourceView(
		backBufferInfo.texture, &srvDesc, &backBufferInfo.srv
	);

	if (FAILED(result)) {
		logger->Error("Failed to create SRV for back buffer");
		return;
	}

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = rendererDevice->CreateSamplerState(
		&samplerDesc, &backBufferInfo.sampler
	);

	if (FAILED(result)) {
		logger->Error("Failed to create sampler state for back buffer");
		return;
	}

	CreateUnownedTexture(BUILTIN_BACKBUFFER_TEXNAME, backBufferInfo);
	logger->Info("Created unowned texture to the backbuffer");
}

void testbed::CreateFeatureTexture(
	const char *name, const FeatureTextureInfo &info
) {
	// Considered a bad logic error
	if (textures.find(name) != textures.end()) {
		logger->Error("Texture with name %s already exists", name);
		return;
	}

	if (!rendererDevice) {
		logger->Error("Renderer device is null");
		return;
	}

	OwnedTexturePtr entry = std::make_shared<TextureEntry<ComPtr>>();
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = info.width;
	textureDesc.Height = info.height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = info.format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags =
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	if (info.shared) {
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
	}

	auto result = rendererDevice->CreateTexture2D(
		&textureDesc, nullptr, entry->texture.GetAddressOf()
	);

	if (FAILED(result)) {
		logger->Error("Failed to create texture with name %s", name);
		return;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = info.format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	result = rendererDevice->CreateRenderTargetView(
		entry->texture.Get(), &rtvDesc, entry->rtv.GetAddressOf()
	);

	if (FAILED(result)) {
		logger->Error("Failed to create RTV for texture with name %s", name);
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = info.format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	result = rendererDevice->CreateShaderResourceView(
		entry->texture.Get(), &srvDesc, entry->srv.GetAddressOf()
	);

	if (FAILED(result)) {
		logger->Error("Failed to create SRV for texture with name %s", name);
		return;
	}

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = rendererDevice->CreateSamplerState(
		&samplerDesc, entry->sampler.GetAddressOf()
	);

	if (FAILED(result)) {
		logger->Error(
			"Failed to create sampler state for texture with name %s", name
		);
		return;
	}

	textures[name] = std::move(entry);
}

void testbed::CreateUnownedTexture(
	const char *name, const UnownedTextureInfo &info
) {
	// Considered a bad logic error
	if (unownedTextures.find(name) != unownedTextures.end()) {
		logger->Error("Unowned texture with name %s already exists", name);
		return;
	}

	UnownedTexturePtr entry = std::make_shared<TextureEntry<UnownedResource>>();
	entry->texture = info.texture;
	entry->rtv = info.rtv;
	entry->srv = info.srv;
	entry->sampler = info.sampler;

	unownedTextures[name] = std::move(entry);
}

ID3D11RenderTargetView *testbed::GetTextureRTV(const char *name) {
	ZoneScoped;
	if (OwnedTextureExists(name)) {
		return GetOwnedTexture(name)->rtv.Get();
	}

	if (UnownedTextureExists(name)) {
		return GetUnownedTexture(name)->rtv;
	}

	logger->Error("Texture with name %s does not exist", name);
	return nullptr;
}

ID3D11ShaderResourceView *testbed::GetTextureSRV(const char *name) {
	ZoneScoped;

	if (OwnedTextureExists(name)) {
		return GetOwnedTexture(name)->srv.Get();
	}

	if (UnownedTextureExists(name)) {
		return GetUnownedTexture(name)->srv;
	}

	logger->Error("Texture with name %s does not exist", name);
	return nullptr;
}

ID3D11SamplerState *testbed::GetTextureSampler(const char *name) {
	ZoneScoped;
	if (OwnedTextureExists(name)) {
		return GetOwnedTexture(name)->sampler.Get();
	}

	if (UnownedTextureExists(name)) {
		return GetUnownedTexture(name)->sampler;
	}

	logger->Error("Texture with name %s does not exist", name);
	return nullptr;
}

HANDLE testbed::GetTextureSharedHandle(const char *name) {
	if (UnownedTextureExists(name)) {
		IDXGIResource *resource;
		if (const auto result =
				GetUnownedTexture(name)->texture->QueryInterface(
					__uuidof(IDXGIResource),
					reinterpret_cast<void **>(&resource)
				);
			FAILED(result)) {
			throw std::runtime_error("Failed to get DXGI resource");
		}

		HANDLE handle;

		if (const auto result = resource->GetSharedHandle(&handle);
			FAILED(result)) {
			throw std::runtime_error("Failed to get shared handle");
		}

		resource->Release();
		return handle;
	}

	if (OwnedTextureExists(name)) {
		// We can use ComPtr's fancy methods here.
		// It will also automatically release the resource when it goes out of
		// scope.
		ComPtr<IDXGIResource> resource;
		if (const auto result = GetOwnedTexture(name)->texture.As(&resource);
			FAILED(result)) {
			throw std::runtime_error("Failed to get DXGI resource");
		}

		HANDLE handle;
		if (const auto result = resource->GetSharedHandle(&handle);
			FAILED(result)) {
			throw std::runtime_error("Failed to get shared handle");
		}

		return handle;
	}

	logger->Error("Texture with name %s does not exist", name);
	return nullptr;
}