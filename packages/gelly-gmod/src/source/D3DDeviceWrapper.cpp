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
const uintptr_t D3DDeviceWrapperStructStart = 0x98000;
const uintptr_t D3DDeviceWrapperStructSize = 0x4000;
const uintptr_t D3DDeviceWrapperOffset = 0x9a928;

IDirect3DDevice9Ex *GetD3DDevice() {
	LOG_INFO("Starting process to get D3D9Ex device.");
	// Grab the BaseShaderAPIDLL module.
	EnsureShaderAPILoaded();

	// Try struct scanning first, if that fails, try the offset.
	LOG_INFO("Starting with a struct scan...");
	auto *d3dDeviceWrapper = reinterpret_cast<D3DDeviceWrapper *>(nullptr);

	const auto structAddress = shaderAPI.ScanStruct({
		.start = D3DDeviceWrapperStructStart,
		.size = D3DDeviceWrapperStructSize,
		.target =
			{
				.members =
					{{
						 // Pointer to IDirect3DDevice9
						 .offset = 0x0,
						 .size = sizeof(IDirect3DDevice9 *),
						 .predicate =
							 [](const void *data, size_t) {
								 uintptr_t address =
									 *static_cast<const uintptr_t *>(data);
								 return address != 0 && address != 0xFFFFFFFF &&
										address > 0x1000;
							 },
					 },
					 {
						 // Single 0x01 byte (not sure what it is, but its
						 // there)
						 .offset = sizeof(IDirect3DDevice9 *),
						 .size = 1,	 // single byte
						 .predicate =
							 [](const void *data, size_t) {
								 return *static_cast<const uint8_t *>(data) ==
										0x01;
							 },
					 },
					 {
						 // 15 bytes of padding
						 .offset = sizeof(IDirect3DDevice9 *) + 1,
						 .size = 15,
						 .predicate = AlwaysNull,
					 },
					 {
						 // 0x07 byte
						 .offset = sizeof(IDirect3DDevice9 *) + 15 + 1,
						 .size = 1,
						 .predicate =
							 [](const void *data, size_t) {
								 return *static_cast<const uint8_t *>(data) ==
										0x07;
							 },
					 }},
			},
	});

	d3dDeviceWrapper = reinterpret_cast<D3DDeviceWrapper *>(structAddress);

	if (!d3dDeviceWrapper) {
		LOG_WARNING("Struct scan failed... trying offset scan.");
		d3dDeviceWrapper =
			shaderAPI.GetObjectAt<D3DDeviceWrapper *>(D3DDeviceWrapperOffset);
	}

	LOG_INFO("Fetched D3DDevice wrapper at address: %p", d3dDeviceWrapper);
	IDirect3DDevice9 *d3d9Device = d3dDeviceWrapper->m_pD3DDevice;
	if (!d3d9Device) {
		LOG_WARNING("Bailing: D3D9 device is null.");
		return nullptr;
	}

	// So, GMod like most source games automatically use D3D9Ex. This is great
	// because Gelly's texture interop will *only* work on D3D9Ex. However, the
	// D3DDeviceWrapper has a D3D9 device. So we need to query for the D3D9Ex
	// device.

	// The major thing about this though is that this functions as a check if
	// the device is D3D9Ex. If it isn't, then we can't use Gelly's texture
	// interop and thus Gelly will refuse to load.

	uintptr_t vtableAddress = *reinterpret_cast<uintptr_t *>(d3d9Device);
	if (!vtableAddress) {
		LOG_WARNING("Bailing: D3D9 device vtable is null.");
		return nullptr;
	}

	IDirect3DDevice9Ex *d3d9DeviceEx = nullptr;
	HRESULT result = d3d9Device->QueryInterface(IID_PPV_ARGS(&d3d9DeviceEx));

	if (FAILED(result)) {
		LOG_WARNING("Failed to query for D3D9Ex device!");
		return nullptr;
	}

	LOG_INFO("Success, fetched D3D9Ex device at address: %p", d3d9DeviceEx);

	return d3d9DeviceEx;
}
