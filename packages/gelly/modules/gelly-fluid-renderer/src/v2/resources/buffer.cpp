#include "buffer.h"

#include <helpers/parse-bind-flags.h>

#include <stdexcept>

namespace gelly {
namespace renderer {
Buffer::Buffer(const BufferCreateInfo &createInfo) : createInfo(createInfo) {
	buffer = CreateBufferResource();

	const auto parsedBindFlags = util::ParseBindFlags(createInfo.bindFlags);

	if (parsedBindFlags.isSRVRequired) {
		shaderResourceView = CreateShaderResourceView(buffer);
	}

	if (parsedBindFlags.isUAVRequired) {
		unorderedAccessView = CreateUnorderedAccessView(buffer);
	}
}

auto Buffer::CreateBuffer(const BufferCreateInfo &&createInfo)
	-> std::shared_ptr<Buffer> {
	return std::make_shared<Buffer>(createInfo);
}

auto Buffer::GetRawBuffer() -> ComPtr<ID3D11Buffer> { return buffer; }

auto Buffer::GetShaderResourceView() -> ComPtr<ID3D11ShaderResourceView> {
	return shaderResourceView;
}

auto Buffer::GetUnorderedAccessView() -> ComPtr<ID3D11UnorderedAccessView> {
	return unorderedAccessView;
}

auto Buffer::GetBufferBindFlags() -> UINT { return createInfo.bindFlags; }

auto Buffer::GetBufferStride() -> UINT { return createInfo.stride; }

auto Buffer::GetCPUAccessFlags() -> UINT { return createInfo.cpuAccessFlags; }

auto Buffer::CreateBufferResource() -> ComPtr<ID3D11Buffer> {
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = createInfo.maxElementCount * createInfo.stride;
	desc.StructureByteStride = createInfo.stride;
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

auto Buffer::CreateShaderResourceView(const ComPtr<ID3D11Buffer> &buffer)
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

auto Buffer::CreateUnorderedAccessView(const ComPtr<ID3D11Buffer> &buffer)
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