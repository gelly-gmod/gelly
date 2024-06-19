
#include "device.h"

#include <helpers/throw-informative-exception.h>

#include <stdexcept>

namespace gelly {
namespace renderer {

Device::Device() : device(nullptr), deviceContext(nullptr) {
	CreateDevice(device);
}

auto Device::GetRawDevice() -> ComPtr<ID3D11Device> { return device; }
auto Device::GetRawDeviceContext() -> ComPtr<ID3D11DeviceContext> {
	return deviceContext;
}

auto Device::CreateDevice(ComPtr<ID3D11Device> &device) -> void {
	auto featureLevel = GetFeatureLevel();
	const auto result = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		GetDeviceFlags(),
		nullptr,
		0,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		&featureLevel,
		deviceContext.GetAddressOf()
	);

	if (FAILED(result)) {
		GELLY_RENDERER_THROW(
			std::runtime_error, "Failed to create D3D11 device!"
		);
	}
}

}  // namespace renderer
}  // namespace gelly