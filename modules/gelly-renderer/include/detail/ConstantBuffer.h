#ifndef GELLY_CONSTANTBUFFER_H
#define GELLY_CONSTANTBUFFER_H
#include <d3d11.h>
#include <detail/ErrorHandling.h>
#include <wrl.h>

using namespace Microsoft::WRL;

template <typename Structure>
class ConstantBuffer {
private:
	ComPtr<ID3D11Buffer> buffer;

public:
	explicit ConstantBuffer(ID3D11Device *device) {
		static_assert(
			(sizeof(Structure) % 16) == 0,
			"Constant Buffer size must be 16-byte aligned"
		);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(Structure);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		DX("Failed to make constant buffer",
		   device->CreateBuffer(&desc, nullptr, buffer.GetAddressOf()));
	}

	~ConstantBuffer() = default;

	void Set(ID3D11DeviceContext *context, Structure *newData) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		DX("Failed to map constant buffer",
		   context->Map(
			   buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource
		   ));
		memcpy(mappedResource.pData, newData, sizeof(Structure));
		context->Unmap(buffer.Get(), 0);
	}

	void BindToShaders(ID3D11DeviceContext *context, int slot) {
		context->PSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
		context->VSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
		context->GSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
	}
};

#endif	// GELLY_CONSTANTBUFFER_H
