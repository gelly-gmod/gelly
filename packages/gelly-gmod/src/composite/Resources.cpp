#include "Resources.h"

#include <stdexcept>

#include "source/D3DDeviceWrapper.h"
#include "source/GetCubemap.h"

UnownedResources Resources::FindGModResources() {
	UnownedResources resources = {};
	resources.device = GetD3DDevice();

	if (!resources.device) {
		throw std::runtime_error("Failed to get D3D9Ex device");
	}

	if (const auto hr = resources.device->GetDirect3D(&resources.d3d9);
		FAILED(hr)) {
		throw std::runtime_error("Failed to get IDirect3D9");
	}

	DisableMaterialSystemThreading();

	return resources;
}
