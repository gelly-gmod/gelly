#include "TextureOverride.h"

#include <d3d9.h>

#include <stdexcept>

#include "source/D3DDeviceWrapper.h"

typedef HRESULT(WINAPI *
					D3DCreateTexture)(IDirect3DDevice9 *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9 **, HANDLE *);

static VTable D3DDeviceVTable;

// We pass arguments globally since the hook can't take custom arguments.
static d3d9::Texture **targetTexture = nullptr;
static D3DFORMAT targetFormat = D3DFMT_UNKNOWN;

static D3DCreateTexture originalCreateTexture = nullptr;

DEFINE_VMT_HOOK(CreateTexture, 23, D3DCreateTexture);
VMT_HOOK_BODY(
	CreateTexture,
	WINAPI,
	HRESULT,
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
	// We automatically disable here so that the hook doesn't get called again.
	// Relieves a little burden off of Lua.
	DISABLE_VMT_HOOK(D3DDeviceVTable, CreateTexture);

	HANDLE shared_handle = nullptr;

	IDirect3DTexture9 *rawTexture = nullptr;
	HRESULT result = VMT_HOOK_CALL_ORIG(
		CreateTexture,
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

	ENABLE_VMT_HOOK(D3DDeviceVTable, CreateTexture);
}

void TextureOverride_Init() {
	IDirect3DDevice9Ex *dev = GetD3DDevice();

	if (dev == nullptr) {
		throw std::runtime_error("Failed to get the D3D9 device.");
	}

	if (MH_Initialize() != MH_OK) {
		throw std::runtime_error("Failed to initialize MinHook.");
	}

	D3DDeviceVTable.Init(reinterpret_cast<void ***>(dev), 119);
	APPLY_VMT_HOOK(D3DDeviceVTable, CreateTexture);
}

void TextureOverride_Shutdown() {
	REMOVE_VMT_HOOK(D3DDeviceVTable, CreateTexture);

	if (MH_Uninitialize() != MH_OK) {
		throw std::runtime_error("Failed to uninitialize MinHook.");
	}
}