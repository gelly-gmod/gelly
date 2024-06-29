#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H
#include <memory>
#include <stdexcept>
#include <type_traits>

#include "device.h"
#include "resources/buffer.h"

namespace gelly {
namespace renderer {
namespace util {

template <typename T>
class ConstantBuffer {
	static_assert(
		sizeof(T) % 16 == 0,
		"ConstantBuffer size must be a multiple of 16 bytes."
	);

	static_assert(std::is_pod_v<T>, "ConstantBuffer type must be a POD type.");

public:
	struct ConstantBufferCreateInfo {
		const std::shared_ptr<Device> device;
	};

	explicit ConstantBuffer(const ConstantBufferCreateInfo &createInfo) :
		createInfo(createInfo), buffer(CreateBuffer()) {
		const auto queryResult = buffer->GetRawBuffer().As(&bufferResource);

		GELLY_RENDERER_THROW_ON_FAIL(
			queryResult,
			std::invalid_argument,
			"Failed to query buffer resource"
		);
	}

	~ConstantBuffer() = default;

	auto GetBuffer() -> std::shared_ptr<Buffer> { return buffer; }

	auto UpdateBuffer(T &data) -> void {
		// constant buffers never support actual subresource updates, so
		// we use the nullptr which just does a smart copy.
		createInfo.device->GetRawDeviceContext()->UpdateSubresource(
			bufferResource.Get(),
			0,
			nullptr,
			reinterpret_cast<void *>(&data),
			0,
			0
		);
	}

private:
	ConstantBufferCreateInfo createInfo;
	std::shared_ptr<Buffer> buffer;
	ComPtr<ID3D11Resource> bufferResource;

	auto CreateBuffer() -> std::shared_ptr<Buffer> {
		const auto bufferCreateInfo =
			Buffer::BufferCreateInfo::WithAutomaticStride<T>(
				{.device = createInfo.device,
				 .maxElementCount = 1,
				 .initialData = nullptr,
				 .usage = D3D11_USAGE_DEFAULT,	// update subresource can copy
												// data to a default buffer
				 .format = DXGI_FORMAT_UNKNOWN,
				 .cpuAccessFlags = 0,
				 .miscFlags = 0,
				 .bindFlags = D3D11_BIND_CONSTANT_BUFFER}
			);

		return Buffer::CreateBuffer(std::move(bufferCreateInfo));
	}
};

}  // namespace util
}  // namespace renderer
}  // namespace gelly

#endif	// CONSTANT_BUFFER_H
