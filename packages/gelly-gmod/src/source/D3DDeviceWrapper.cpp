#include "D3DDeviceWrapper.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "hooking/Library.h"
#include "logging/global-macros.h"

static Library shaderAPI;

void EnsureShaderAPILoaded() {
	if (shaderAPI.IsInitialized()) {
		return;
	}

	shaderAPI.Init("shaderapidx9.dll");
}

/**
 * Current offset to D3DDevice in GMod.
 */
const uintptr_t D3DDeviceWrapperOffset = 0x9A9D8;
IDirect3DDevice9Ex *GetD3DDevice() {
	LOG_INFO("Starting process to get D3D9Ex device.");
	// Grab the BaseShaderAPIDLL module.
	EnsureShaderAPILoaded();

	const auto *d3dDeviceWrapper =
		shaderAPI.GetObjectAt<D3DDeviceWrapper *>(D3DDeviceWrapperOffset);

	if (!d3dDeviceWrapper) {
		LOG_WARNING("Bailing: D3DDeviceWrapper is null.");
		return nullptr;
	}

	LOG_INFO("Fetched D3DDevice wrapper at address: %p", d3dDeviceWrapper);
	IDirect3DDevice9 *d3d9Device = d3dDeviceWrapper->m_pD3DDevice;

	// So, GMod like most source games automatically use D3D9Ex. This is great
	// because Gelly's texture interop will *only* work on D3D9Ex. However, the
	// D3DDeviceWrapper has a D3D9 device. So we need to query for the D3D9Ex
	// device.

	// The major thing about this though is that this functions as a check if
	// the device is D3D9Ex. If it isn't, then we can't use Gelly's texture
	// interop and thus Gelly will refuse to load.

	IDirect3DDevice9Ex *d3d9DeviceEx = nullptr;
	HRESULT result = d3d9Device->QueryInterface(IID_PPV_ARGS(&d3d9DeviceEx));

	if (FAILED(result)) {
		LOG_WARNING("Failed to query for D3D9Ex device!");
		return nullptr;
	}

	LOG_INFO("Success, fetched D3D9Ex device at address: %p", d3d9DeviceEx);

	return d3d9DeviceEx;
}
