
#include "device.h"

#include <helpers/throw-informative-exception.h>

#include <stdexcept>

namespace gelly {
namespace renderer {

Device::Device()
	: device(nullptr), deviceContext(nullptr), performanceMarker(nullptr) {
	CreateDevice(device);
	QueryForPerformanceMarker(performanceMarker);
}

auto Device::GetRawDevice() -> ComPtr<ID3D11Device> { return device; }
auto Device::GetRawDeviceContext() -> ComPtr<ID3D11DeviceContext> {
	return deviceContext;
}
auto Device::GetPerformanceMarker() -> ComPtr<ID3DUserDefinedAnnotation> {
	return performanceMarker;
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

auto Device::QueryForPerformanceMarker(
	ComPtr<ID3DUserDefinedAnnotation> &performanceMarker
) -> void {
	if (FAILED(deviceContext->QueryInterface(
			IID_PPV_ARGS(performanceMarker.GetAddressOf())
		))) {
		GELLY_RENDERER_THROW(
			std::runtime_error, "Failed to create perf marker"
		);
	}
}

auto Device::GetFeatureLevel() -> D3D_FEATURE_LEVEL {
	return D3D_FEATURE_LEVEL_11_1;
}

auto Device::GetDeviceFlags() -> D3D11_CREATE_DEVICE_FLAG {
	return D3D11_CREATE_DEVICE_SINGLETHREADED;
}

}  // namespace renderer
}  // namespace gelly