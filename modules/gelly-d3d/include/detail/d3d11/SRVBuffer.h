#ifndef GELLY_SRVBUFFER_H
#define GELLY_SRVBUFFER_H

#include <d3d11.h>
#include <wrl.h>

#include "Buffer.h"
#include "ErrorHandling.h"

using namespace Microsoft::WRL;

namespace d3d11 {
template <typename T>
class SRVBuffer {
private:
	ComPtr<ID3D11ShaderResourceView> view;

public:
	SRVBuffer(ID3D11Device *device, const Buffer<T> &buffer) {
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.NumElements = buffer.GetCapacity();
		desc.Buffer.ElementWidth = sizeof(T);

		DX("Failed to create SRV!",
		   device->CreateShaderResourceView(
			   buffer.Get(), &desc, view.GetAddressOf()
		   ));
	}

	SRVBuffer(
		ID3D11Device *device, ID3D11Buffer *bufferReference, int maxCapacity
	) {
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.NumElements = maxCapacity;
		desc.Buffer.ElementWidth = sizeof(T);

		DX("Failed to create SRV!",
		   device->CreateShaderResourceView(
			   bufferReference, &desc, view.GetAddressOf()
		   ));
	}

	~SRVBuffer() = default;

	[[nodiscard]] ID3D11ShaderResourceView *Get() const { return view.Get(); }
};
}  // namespace d3d11
#endif	// GELLY_SRVBUFFER_H
