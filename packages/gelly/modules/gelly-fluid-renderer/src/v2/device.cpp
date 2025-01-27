
#include "device.h"

#include <helpers/throw-informative-exception.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace gelly {
namespace renderer {

Device::Device() :
	device(nullptr), deviceContext(nullptr), performanceMarker(nullptr) {
	CreateDevice(device);
	QueryForPerformanceMarker(performanceMarker);

#ifdef GELLY_USE_DEBUG_LAYER
	debug = CreateDebugLayer();
	infoQueue = CreateInfoQueue(debug);
#endif
}

auto Device::GetRawDevice() -> ComPtr<ID3D11Device> { return device; }
auto Device::GetRawDeviceContext() -> ComPtr<ID3D11DeviceContext> {
	return deviceContext;
}
auto Device::GetPerformanceMarker() -> ComPtr<ID3DUserDefinedAnnotation> {
	return performanceMarker;
}

#ifdef GELLY_USE_DEBUG_LAYER
auto Device::OutputAllDebugMessages() -> void {
	UINT64 messageCount = infoQueue->GetNumStoredMessages();

	for (UINT64 i = 0; i < messageCount; ++i) {
		SIZE_T messageLength = 0;
		infoQueue->GetMessage(i, nullptr, &messageLength);

		const auto message =
			static_cast<D3D11_MESSAGE *>(malloc(messageLength));

		infoQueue->GetMessage(i, message, &messageLength);
		OutputDebugStringA(GELLY_FUNCTION_NAME
						   ": Sending a D3D11 event on the next message!\n");
		OutputDebugStringA(message->pDescription);
	}
}
#endif

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
#ifdef GELLY_USE_DEBUG_LAYER
	return D3D11_CREATE_DEVICE_DEBUG;
#else
	return D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif
}

#ifdef GELLY_USE_DEBUG_LAYER
auto Device::CreateDebugLayer() -> ComPtr<ID3D11Debug> {
	ComPtr<ID3D11Debug> debug;
	const auto castResult = device.As(&debug);

	GELLY_RENDERER_THROW_ON_FAIL(
		castResult,
		std::runtime_error,
		"Failed to cast device to debug faciltiy"
	);

	return debug;
}

auto Device::CreateInfoQueue(ComPtr<ID3D11Debug> &debug)
	-> ComPtr<ID3D11InfoQueue> {
	ComPtr<ID3D11InfoQueue> infoQueue;
	const auto castResult = debug.As(&infoQueue);

	GELLY_RENDERER_THROW_ON_FAIL(
		castResult, std::runtime_error, "Failed to cast debug to info queue"
	);

	return infoQueue;
}

#endif
}  // namespace renderer
}  // namespace gelly