#ifndef GELLYD3D_D3D11_TEXTURE_H
#define GELLYD3D_D3D11_TEXTURE_H

#include <d3d11.h>
#include <wrl.h>

#include "ErrorHandling.h"

using namespace Microsoft::WRL;

namespace d3d11 {
template <typename T>
class Buffer {
private:
	ComPtr<ID3D11Buffer> buffer;

public:
	/**
	 * @brief Construct a new Buffer object with initial data.
	 * @param maxCapacity Maximum capacity of the buffer. (in # of elements)
	 * @param initData Optional initial data to fill the buffer with. You must
	 * take care of the allocation and deallocation of this data.
	 * @param bindFlags Bind flags for the buffer.
	 */
	Buffer(
		ID3D11Device *device,
		int maxCapacity,
		T *initData,
		D3D11_BIND_FLAG bindFlags
	);

	void SetAsVB(
		ID3D11DeviceContext *context, ID3D11InputLayout *layout, int slot
	) const;
	[[nodiscard]] ID3D11Buffer *Get() const;
};

// We have to define the template functions in the header file.
template <typename T>
Buffer<T>::Buffer(
	ID3D11Device *device,
	int maxCapacity,
	T *initData,
	D3D11_BIND_FLAG bindFlags
) {
	D3D11_BUFFER_DESC desc{};
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(T) * maxCapacity;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data{};
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = initData;

	DX("Failed to create buffer!",
	   device->CreateBuffer(
		   &desc, initData ? &data : nullptr, buffer.GetAddressOf()
	   ));
}

template <typename T>
void Buffer<T>::SetAsVB(
	ID3D11DeviceContext *context, ID3D11InputLayout *layout, int slot
) const {
	UINT stride = sizeof(T);
	UINT offset = 0;
	ID3D11Buffer *bufferPtr = buffer.Get();
	context->IASetVertexBuffers(slot, 1, &bufferPtr, &stride, &offset);
	context->IASetInputLayout(layout);
}

template <typename T>
ID3D11Buffer *Buffer<T>::Get() const {
	return buffer.Get();
}

}  // namespace d3d11

#endif	// GELLYD3D_D3D11_TEXTURE_H