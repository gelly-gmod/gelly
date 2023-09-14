#include "TextureOverride.h"

#include <d3d9.h>

#include <stdexcept>

#include "source/D3DDeviceWrapper.h"

typedef HRESULT(WINAPI *
					D3DCreateTexture)(IDirect3DDevice9 *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9 **, HANDLE *);

// We pass arguments globally since the hook can't take custom arguments.
static d3d9::Texture **targetTexture = nullptr;
static D3DFORMAT targetFormat = D3DFMT_UNKNOWN;

static D3DCreateTexture originalCreateTexture = nullptr;
static void *createTexturePtr = nullptr;

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
	// Disable the hook so we don't override the wrong texture.
	if (MH_DisableHook(createTexturePtr) != MH_OK) {
		throw std::runtime_error("Failed to disable the CreateTexture hook.");
	}

	HANDLE shared_handle = nullptr;

	IDirect3DTexture9 *rawTexture = nullptr;
	HRESULT result = originalCreateTexture(
		device,
		width,
		height,
		1,
		D3DUSAGE_RENDERTARGET,
		targetFormat,
		D3DPOOL_DEFAULT,
		&rawTexture,
		&shared_handle
	);

	if (FAILED(result)) {
		return result;
	}

	*targetTexture = new d3d9::Texture(
		rawTexture, shared_handle, (int)width, (int)height, targetFormat
	);

	// Give caller the texture we created.
	*texture = rawTexture;
	return result;
}

void TextureOverride_GetTexture(d3d9::Texture **texture, D3DFORMAT format) {
	targetTexture = texture;
	targetFormat = format;

	if (MH_EnableHook(createTexturePtr) != MH_OK) {
		throw std::runtime_error("Failed to enable the CreateTexture hook.");
	}
}

void TextureOverride_Init() {
	IDirect3DDevice9Ex *dev = GetD3DDevice();

	if (dev == nullptr) {
		throw std::runtime_error("Failed to get the D3D9 device.");
	}

	if (MH_Initialize() != MH_OK) {
		throw std::runtime_error("Failed to initialize MinHook.");
	}

	void **d3d9VTable = *reinterpret_cast<void ***>(dev);

	// We're going to hook IDirect3DDevice9::CreateTexture
	// This function is at index 23 in the vtable.
	createTexturePtr = d3d9VTable[23];

	if (MH_CreateHook(
			createTexturePtr,
			(LPVOID)&HookedD3DCreateTexture,
			reinterpret_cast<void **>(&originalCreateTexture)
		) != MH_OK) {
		throw std::runtime_error("Failed to create the CreateTexture hook.");
	}
}

void TextureOverride_Shutdown() {
	if (MH_DisableHook(createTexturePtr) != MH_OK) {
		throw std::runtime_error("Failed to disable the CreateTexture hook.");
	}

	if (MH_Uninitialize() != MH_OK) {
		throw std::runtime_error("Failed to uninitialize MinHook.");
	}
}