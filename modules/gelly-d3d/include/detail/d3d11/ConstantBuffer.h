#ifndef GELLY_D3D11CONSTANTBUFFER_H
#define GELLY_D3D11CONSTANTBUFFER_H

#include <d3d11.h>

#include "Buffer.h"

namespace d3d11 {
template <typename Structure>
class ConstantBuffer {
private:
	Buffer<Structure> buffer;

public:
	explicit ConstantBuffer(ID3D11Device *device);
	~ConstantBuffer() = default;

	void Set(ID3D11DeviceContext *context, Structure *newData);
	void BindToShaders(ID3D11DeviceContext *context, int slot);
};

template <typename Structure>
ConstantBuffer<Structure>::ConstantBuffer(ID3D11Device *device)
	: buffer(
		  device,
		  1,
		  nullptr,
		  D3D11_BIND_CONSTANT_BUFFER,
		  D3D11_USAGE_DYNAMIC,
		  D3D11_CPU_ACCESS_WRITE
	  ) {
	static_assert(
		(sizeof(Structure) % 16) == 0,
		"Constant Buffer size must be 16-byte aligned"
	);
}

template <typename Structure>
void ConstantBuffer<Structure>::Set(
	ID3D11DeviceContext *context, Structure *newData
) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DX("Failed to map constant buffer",
	   context->Map(
		   buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource
	   ));
	memcpy(mappedResource.pData, newData, sizeof(Structure));
	context->Unmap(buffer.Get(), 0);
}

template <typename Structure>
void ConstantBuffer<Structure>::BindToShaders(
	ID3D11DeviceContext *context, int slot
) {
	ID3D11Buffer *buffers[1] = {buffer.Get()};
	context->PSSetConstantBuffers(slot, 1, buffers);
	context->VSSetConstantBuffers(slot, 1, buffers);
	context->GSSetConstantBuffers(slot, 1, buffers);
}

};	// namespace d3d11

#endif	// GELLY_D3D11CONSTANTBUFFER_H
