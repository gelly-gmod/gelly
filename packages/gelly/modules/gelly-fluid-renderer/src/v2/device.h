#ifndef DEVICE_H
#define DEVICE_H

#include <d3d11.h>
#include <d3d11_1.h>
#include <helpers/comptr.h>

namespace gelly {
namespace renderer {

/**
 * Encapsulates the D3D11 device and device context, and also exposes resource
 * management.
 */
class Device {
public:
	Device();
	~Device() = default;

	auto GetRawDevice() -> ComPtr<ID3D11Device>;
	auto GetRawDeviceContext() -> ComPtr<ID3D11DeviceContext>;
	auto GetPerformanceMarker() -> ComPtr<ID3DUserDefinedAnnotation>;

#ifdef GELLY_USE_DEBUG_LAYER
	/**
	 * Notice the word "output," we don't print. Therefore, you must use a
	 * debugger or a program like DbgView to see the output.
	 */

	auto OutputAllDebugMessages() -> void;
#endif

private:
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
	ComPtr<ID3DUserDefinedAnnotation> performanceMarker;

#ifdef GELLY_USE_DEBUG_LAYER
	ComPtr<ID3D11Debug> debug;
	ComPtr<ID3D11InfoQueue> infoQueue;
#endif

	auto CreateDevice(ComPtr<ID3D11Device> &device) -> void;
	auto QueryForPerformanceMarker(
		ComPtr<ID3DUserDefinedAnnotation> &performanceMarker
	) -> void;
	auto GetDeviceFlags() -> D3D11_CREATE_DEVICE_FLAG;
	auto GetFeatureLevel() -> D3D_FEATURE_LEVEL;

#ifdef GELLY_USE_DEBUG_LAYER
	auto CreateDebugLayer() -> ComPtr<ID3D11Debug>;
	auto CreateInfoQueue(ComPtr<ID3D11Debug> &debug) -> ComPtr<ID3D11InfoQueue>;
#endif
};

}  // namespace renderer
}  // namespace gelly

#endif	// DEVICE_H
