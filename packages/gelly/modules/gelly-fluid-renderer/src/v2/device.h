#ifndef DEVICE_H
#define DEVICE_H

#include <d3d11.h>
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

private:
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;

	auto CreateDevice(ComPtr<ID3D11Device> &device) -> void;
	auto GetDeviceFlags() -> D3D11_CREATE_DEVICE_FLAG;
	auto GetFeatureLevel() -> D3D_FEATURE_LEVEL;
};

}  // namespace renderer
}  // namespace gelly

#endif	// DEVICE_H
