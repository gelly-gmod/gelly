
#include "Textures.h"

#include <wrl.h>

#include <memory>
#include <unordered_map>

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
};

using OwnedTexturePtr = std::shared_ptr<TextureEntry<ComPtr>>;
using UnownedTexturePtr = std::shared_ptr<TextureEntry<UnownedResource>>;

static std::unordered_map<const char *, OwnedTexturePtr> textures;
static std::unordered_map<const char *, UnownedTexturePtr> unownedTextures;

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

	unownedTextures[name] = std::move(entry);
}

ID3D11RenderTargetView *testbed::GetTextureRTV(const char *name) {
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
	if (OwnedTextureExists(name)) {
		return GetOwnedTexture(name)->srv.Get();
	}

	if (UnownedTextureExists(name)) {
		return GetUnownedTexture(name)->srv;
	}

	logger->Error("Texture with name %s does not exist", name);
	return nullptr;
}