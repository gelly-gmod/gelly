#include "D3DDeviceWrapper.h"

#include <TlHelp32.h>
#include <windows.h>

uintptr_t get_shaderapi_base_address() {
	// Take a snapshot of the GMod process. We want to see what modules are
	// loaded into the process.

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

	if (snapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	// Look for the shader API module.

	MODULEENTRY32 module_entry;
	module_entry.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(snapshot, &module_entry)) {
		CloseHandle(snapshot);
		return 0;
	}

	do {
		if (strcmp(module_entry.szModule, "shaderapidx9.dll") == 0) {
			CloseHandle(snapshot);
			return (uintptr_t)module_entry.modBaseAddr;
		}
	} while (Module32Next(snapshot, &module_entry));

	CloseHandle(snapshot);
	return 0;
}

/**
 * Current offset to D3DDevice in GMod.
 */
const uintptr_t D3DDeviceWrapperOffset = 0x9A9E0;

IDirect3DDevice9Ex *GetD3DDevice() {
	// Grab the BaseShaderAPIDLL module.
	uintptr_t baseShaderAPIDLL = get_shaderapi_base_address();
	if (!baseShaderAPIDLL) {
		return nullptr;
	}

	auto *d3dDeviceWrapper =
		(D3DDeviceWrapper *)(baseShaderAPIDLL + D3DDeviceWrapperOffset);

	if (!d3dDeviceWrapper) {
		return nullptr;
	}

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
		return nullptr;
	}

	return d3d9DeviceEx;
}