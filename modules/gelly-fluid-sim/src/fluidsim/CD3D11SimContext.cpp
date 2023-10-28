#include "fluidsim/CD3D11SimContext.h"

CD3D11SimContext::CD3D11SimContext(
	ID3D11Device *device, ID3D11DeviceContext *deviceContext
)
	: device(device), deviceContext(deviceContext) {}

void *CD3D11SimContext::GetRenderAPIHandle(RenderAPIHandle handle) {
	switch (handle) {
		case RenderAPIHandle::D3D11Device:
			return device;
		case RenderAPIHandle::D3D11DeviceContext:
			return deviceContext;
		default:
			return nullptr;
	}
}