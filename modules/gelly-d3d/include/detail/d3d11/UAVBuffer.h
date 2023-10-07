#ifndef GELLY_UAVBUFFER_H
#define GELLY_UAVBUFFER_H

#include <d3d11.h>
#include <wrl.h>

#include "Buffer.h"
#include "ErrorHandling.h"

using namespace Microsoft::WRL;

namespace d3d11 {
template <typename T>
class UAVBuffer {
private:
	ComPtr<ID3D11UnorderedAccessView> view;

public:
	UAVBuffer(ID3D11Device *device, const Buffer<T> &buffer) {
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = buffer.GetCapacity();
		desc.Buffer.Flags = 0;

		DX("Failed to create UAV!",
		   device->CreateUnorderedAccessView(
			   buffer.Get(), &desc, view.GetAddressOf()
		   ));
	}

	~UAVBuffer() = default;

	[[nodiscard]] ID3D11UnorderedAccessView *Get() const { return view.Get(); }
};
}  // namespace d3d11
#endif	// GELLY_UAVBUFFER_H
