#ifndef GELLY_IMANAGEDBUFFER_H
#define GELLY_IMANAGEDBUFFER_H

#include <cstdint>

#include "GellyInterface.h"
#include "IManagedShader.h"

namespace Gelly {
enum class BufferType : uint8_t {
	NONE = 0b000,
	VERTEX,
	CONSTANT,
	SHADER_RESOURCE,
	UNORDERED_ACCESS,
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
	uint16_t stride{};
	BufferType type{};
	BufferUsage usage{};
	void *initialData{};
};
}  // namespace Gelly

using namespace Gelly;

constexpr enum BufferType operator&(
	const enum BufferType a, const enum BufferType b
) {
	return static_cast<BufferType>(
		static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
	);
}

constexpr enum BufferType operator|(
	const enum BufferType a, const enum BufferType b
) {
	return static_cast<BufferType>(
		static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
	);
}

constexpr bool operator==(const enum BufferType a, const int b) {
	return static_cast<uint8_t>(a) == static_cast<uint8_t>(b);
}

constexpr enum BufferUsage operator&(
	const enum BufferUsage a, const enum BufferUsage b
) {
	return static_cast<BufferUsage>(
		static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
	);
}

constexpr enum BufferUsage operator|(
	const enum BufferUsage a, const enum BufferUsage b
) {
	return static_cast<BufferUsage>(
		static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
	);
}

constexpr bool operator==(const enum BufferUsage a, const int b) {
	return static_cast<uint8_t>(a) == static_cast<uint8_t>(b);
}

class IRenderContext;

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

	virtual void BindToPipeline(ShaderType shaderType, uint8_t slot) = 0;
};

#endif	// GELLY_IMANAGEDBUFFER_H
