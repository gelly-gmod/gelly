#include "TextureOverride.h"

#include <d3d9.h>

#include <stdexcept>

#include "source/D3DDeviceWrapper.h"

static HRESULT WINAPI HookedD3DCreateTexture(
	IDirect3DDevice9 *device,
	UINT width,
	UINT height,
	UINT levels,
	DWORD usage,
	D3DFORMAT format,
	D3DPOOL pool,
	IDirect3DTexture9 **texture,
	HANDLE *sharedHandle
) {
	using namespace TextureOverride;
	TextureOverride::device = device;

	if (target == TextureOverrideTarget::None) {
		// This shouldn't happen as the hook should be disabled.
		return originalCreateTexture(
			device,
			width,
			height,
			levels,
			usage,
			format,
			pool,
			texture,
			sharedHandle
		);
	}

	HANDLE shared_handle = nullptr;
	d3d9::Texture **target_texture_ptr = nullptr;

	D3DFORMAT texFormat = format;
	switch (target) {
		case TextureOverrideTarget::Normal:
			target_texture_ptr = &sharedTextures.normal;
			texFormat = D3DFMT_A16B16G16R16F;
			break;
		case TextureOverrideTarget::Depth:
			target_texture_ptr = &sharedTextures.depth;
			texFormat = D3DFMT_A16B16G16R16F;
			break;
		default:
			break;
	}

	IDirect3DTexture9 *target_texture = nullptr;
	HRESULT result = originalCreateTexture(
		device,
		width,
		height,
		1,
		D3DUSAGE_RENDERTARGET,
		texFormat,
		D3DPOOL_DEFAULT,
		&target_texture,
		&shared_handle
	);

	if (FAILED(result)) {
		return result;
	}

	*target_texture_ptr = new d3d9::Texture(
		target_texture, shared_handle, (int)width, (int)height, texFormat
	);

	// Give caller the texture we created.
	*texture = target_texture;
	return result;
}

static D3DCreateTexture createTexturePtr = nullptr;

namespace TextureOverride {
D3DCreateTexture originalCreateTexture = nullptr;
SharedTextures sharedTextures{};
TextureOverrideTarget target = TextureOverrideTarget::None;
IDirect3DDevice9 *device = nullptr;
}  // namespace TextureOverride

void TextureOverride::Initialize() {
	IDirect3DDevice9 *dev = GetD3DDevice();

	if (dev == nullptr) {
		throw std::runtime_error("Failed to get the D3D9 device.");
	}

	if (MH_Initialize() != MH_OK) {
		throw std::runtime_error("Failed to initialize MinHook.");
	}

	void **d3d9VTable = *reinterpret_cast<void ***>(dev);

	// We're going to hook IDirect3DDevice9::CreateTexture
	// This function is at index 23 in the vtable.
	createTexturePtr = reinterpret_cast<D3DCreateTexture>(d3d9VTable[23]);

	if (MH_CreateHook(
			createTexturePtr,
			&HookedD3DCreateTexture,
			reinterpret_cast<void **>(&originalCreateTexture)
		) != MH_OK) {
		throw std::runtime_error("Failed to create the CreateTexture hook.");
	}
}

void TextureOverride::Shutdown() {
	if (MH_DisableHook(createTexturePtr) != MH_OK) {
		throw std::runtime_error("Failed to disable the CreateTexture hook.");
	}

	if (MH_Uninitialize() != MH_OK) {
		throw std::runtime_error("Failed to uninitialize MinHook.");
	}

	if (sharedTextures.normal != nullptr) {
		delete sharedTextures.normal;
		sharedTextures.normal = nullptr;
	}

	if (sharedTextures.depth != nullptr) {
		delete sharedTextures.depth;
		sharedTextures.depth = nullptr;
	}
}

void TextureOverride::Enable(TextureOverrideTarget overrideTarget) {
	target = overrideTarget;
	if (MH_EnableHook(createTexturePtr) != MH_OK) {
		throw std::runtime_error("Failed to enable the CreateTexture hook.");
	}
}

void TextureOverride::Disable() {
	target = TextureOverrideTarget::None;
	if (MH_DisableHook(createTexturePtr) != MH_OK) {
		throw std::runtime_error("Failed to disable the CreateTexture hook.");
	}
}