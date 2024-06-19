#include "buffer.h"

#include <helpers/parse-bind-flags.h>

#include <stdexcept>

namespace gelly {
namespace renderer {
template <typename T>
Buffer<T>::Buffer(const BufferCreateInfo &createInfo) : createInfo(createInfo) {
	buffer = CreateBuffer();

	const auto parsedBindFlags = util::ParseBindFlags(createInfo.bindFlags);

	if (parsedBindFlags.isSRVRequired) {
		shaderResourceView = CreateShaderResourceView(buffer);
	}

	if (parsedBindFlags.isUAVRequired) {
		unorderedAccessView = CreateUnorderedAccessView(buffer);
	}
}

template <typename T>
auto Buffer<T>::GetRawBuffer() -> ComPtr<ID3D11Buffer> {
	return buffer;
}

template <typename T>
auto Buffer<T>::GetShaderResourceView() -> ComPtr<ID3D11ShaderResourceView> {
	return shaderResourceView;
}

template <typename T>
auto Buffer<T>::GetUnorderedAccessView() -> ComPtr<ID3D11UnorderedAccessView> {
	return unorderedAccessView;
}

template <typename T>
auto Buffer<T>::CreateBuffer() -> ComPtr<ID3D11Buffer> {
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = createInfo.maxElementCount * STRIDE;
	desc.StructureByteStride = STRIDE;
	desc.Usage = createInfo.usage;
	desc.BindFlags = createInfo.bindFlags;
	desc.CPUAccessFlags = createInfo.cpuAccessFlags;
	desc.MiscFlags = createInfo.miscFlags;

	const auto result = createInfo.device->GetRawDevice()->CreateBuffer(
		&desc, nullptr, buffer.GetAddressOf()
	);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to create buffer"
	);
}

template <typename T>
auto Buffer<T>::CreateShaderResourceView(const ComPtr<ID3D11Buffer> &buffer)
	-> ComPtr<ID3D11ShaderResourceView> {
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = createInfo.format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = createInfo.maxElementCount;

	ComPtr<ID3D11ShaderResourceView> srv;
	const auto result =
		createInfo.device->GetRawDevice()->CreateShaderResourceView(
			buffer.Get(), &desc, srv.GetAddressOf()
		);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to create shader resource view"
	);
}

template <typename T>
auto Buffer<T>::CreateUnorderedAccessView(const ComPtr<ID3D11Buffer> &buffer)
	-> ComPtr<ID3D11UnorderedAccessView> {
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = createInfo.format;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = createInfo.maxElementCount;

	ComPtr<ID3D11UnorderedAccessView> uav;
	const auto result =
		createInfo.device->GetRawDevice()->CreateUnorderedAccessView(
			buffer.Get(), &desc, uav.GetAddressOf()
		);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to create unordered access view"
	);
}
}  // namespace renderer
}  // namespace gelly