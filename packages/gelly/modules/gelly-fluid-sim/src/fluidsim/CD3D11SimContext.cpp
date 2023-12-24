#include "fluidsim/CD3D11SimContext.h"

#include <stdexcept>

CD3D11SimContext::CD3D11SimContext()
	: device(nullptr), deviceContext(nullptr) {}

SimContextAPI CD3D11SimContext::GetAPI() { return SimContextAPI::D3D11; }

void CD3D11SimContext::SetAPIHandle(SimContextHandle handle, void *value) {
	switch (handle) {
		case SimContextHandle::D3D11_DEVICE:
			device = static_cast<ID3D11Device *>(value);
			break;
		case SimContextHandle::D3D11_DEVICE_CONTEXT:
			deviceContext = static_cast<ID3D11DeviceContext *>(value);
			break;
		default:
			throw std::runtime_error(
				"CD3D11SimContext::SetAPIHandle: Invalid handle."
			);
	}
}

void *CD3D11SimContext::GetAPIHandle(SimContextHandle handle) {
	switch (handle) {
		case SimContextHandle::D3D11_DEVICE:
			return device;
		case SimContextHandle::D3D11_DEVICE_CONTEXT:
			return deviceContext;
		default:
			throw std::runtime_error(
				"CD3D11SimContext::GetAPIHandle: Invalid handle."
			);
	}
}