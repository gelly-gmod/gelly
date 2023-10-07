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
	UAVBuffer(ID3D11Device *device, const Buffer<T> &buffer);
	~UAVBuffer() = default;

	[[nodiscard]] ID3D11UnorderedAccessView *Get() const;
};
}  // namespace d3d11
#endif	// GELLY_UAVBUFFER_H
