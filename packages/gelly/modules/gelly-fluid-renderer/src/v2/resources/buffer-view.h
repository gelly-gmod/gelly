#ifndef BUFFER_VIEW_H
#define BUFFER_VIEW_H

#include <device.h>
#include <resources/buffer.h>

#include <memory>

namespace gelly {
namespace renderer {

/**
 * Allows for reading/writing to a buffer.
 * \note This class is a RAII construct. Once out of scope, the buffer view is
 * destroyed and the buffer is unmapped. Likewise, upon initialization, the
 * buffer is mapped. *THIS* is costly, so do not hold a reference to this class
 * for longer than necessary, stack allocate it if possible.
 *
 * Unless a catastrophic error occurs, writing/reading is guaranteed to be safe
 * as long as you hold a reference/lvalue to the buffer view, not a pointer.
 */
class BufferView {
public:
	struct BufferViewCreateInfo {
		const std::shared_ptr<Device> device;
		const std::shared_ptr<Buffer> buffer;
		D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
	};

	explicit BufferView(const BufferViewCreateInfo &createInfo);
	~BufferView();

	template <typename T>
	explicit operator T *() {
		return static_cast<T *>(hostPointer);
	}

	template <typename T>
	auto operator[](unsigned int index) -> T & {
		return static_cast<T *>(hostPointer)[index];
	}

	template <typename T>
	auto operator[](unsigned int index) const -> const T & {
		return static_cast<T *>(hostPointer)[index];
	}

	template <typename T>
	auto Write(unsigned int index, const T &value) -> void {
		static_cast<T *>(hostPointer)[index] = value;
	}

private:
	BufferViewCreateInfo createInfo;
	void *hostPointer;

	auto MapBuffer() -> void *;
	auto UnmapBuffer() -> void;
	auto ThrowIfCPUAccessIsDenied() -> void;
};

}  // namespace renderer
}  // namespace gelly

#endif	// BUFFER_VIEW_H
