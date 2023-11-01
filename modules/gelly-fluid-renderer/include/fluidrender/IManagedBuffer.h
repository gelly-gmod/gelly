#ifndef GELLY_IMANAGEDBUFFER_H
#define GELLY_IMANAGEDBUFFER_H

#include <cstdint>

#include "GellyInterface.h"
#include "IRenderContext.h"

namespace Gelly {
enum class BufferType : uint8_t {
	VERTEX,
	CONSTANT,
};

enum class BufferUsage : uint8_t {
	DEFAULT,
	/**
	 * You should choose this if you're planning on
	 * making a buffer which is read by the GPU
	 * and written to by the CPU.
	 */
	DYNAMIC,
};

struct BufferDesc {
	uint8_t byteWidth{};
	BufferType type{};
	BufferUsage usage{};
	void *initialData{};
};
}  // namespace Gelly

using namespace Gelly;

gelly_interface IManagedBuffer {
public:
	virtual ~IManagedBuffer() = 0;

	virtual void SetDesc(const BufferDesc &desc) = 0;
	[[nodiscard]] virtual const BufferDesc &GetDesc() const = 0;

	virtual bool Create() = 0;
	virtual void Destroy() = 0;
	virtual void AttachToContext(IRenderContext * context) = 0;

	/**
	 * Returns the underlying rendering API buffer.
	 * Do not hold on to this pointer, as it may be invalidated.
	 * @return The underlying buffer.
	 */
	virtual void *GetBufferResource() = 0;
};

#endif	// GELLY_IMANAGEDBUFFER_H
