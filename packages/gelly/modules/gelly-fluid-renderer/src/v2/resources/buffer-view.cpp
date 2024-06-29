#include "buffer-view.h"

#include <stdexcept>

namespace gelly {
namespace renderer {
BufferView::BufferView(const BufferViewCreateInfo &createInfo)
	: hostPointer(MapBuffer()), createInfo(createInfo) {}

BufferView::~BufferView() {
	UnmapBuffer();
	hostPointer = nullptr;
}

auto BufferView::MapBuffer() -> void * {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	const auto result = createInfo.device->GetRawDeviceContext()->Map(
		createInfo.buffer->GetRawBuffer().Get(),
		0,
		createInfo.mapType,
		0,
		&mappedResource
	);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to map buffer"
	);

	return mappedResource.pData;
}

auto BufferView::UnmapBuffer() -> void {
	createInfo.device->GetRawDeviceContext()->Unmap(
		createInfo.buffer->GetRawBuffer().Get(), 0
	);
}

auto BufferView::ThrowIfCPUAccessIsDenied() -> void {
	if (createInfo.buffer->GetCPUAccessFlags() == 0) {
		GELLY_RENDERER_THROW(std::runtime_error, "Buffer has no CPU access");
	}
}

}  // namespace renderer
}  // namespace gelly