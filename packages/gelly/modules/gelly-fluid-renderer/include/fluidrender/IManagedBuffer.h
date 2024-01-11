#ifndef GELLY_IMANAGEDBUFFER_H
#define GELLY_IMANAGEDBUFFER_H

#include <cstdint>
#include <functional>

#include "GellyInterface.h"
#include "IManagedShader.h"
#include "IManagedTexture.h"

namespace Gelly {
using BufferFormat = TextureFormat;

enum BufferType : uint8_t {
	NONE = 0b0000,
	VERTEX = 0b0001,
	CONSTANT = 0b0010,
	SHADER_RESOURCE = 0b0100,
	UNORDERED_ACCESS = 0b1000,
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
	uint32_t byteWidth{};
	uint16_t stride{};
	BufferType type{};
	BufferUsage usage{};
	BufferFormat format{};
	const void *initialData{};
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
	using RawBufferPtr = void *;
	using ModifierFn = std::function<void(RawBufferPtr)>;

	virtual ~IManagedBuffer() = default;

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

	/**
	 * \brief Allows you to modify the buffer data. Anything in the modifier
	 * should not be held on to.
	 * \note Will throw if the buffer is not dynamic.
	 * \param modifier A function which takes a
	 * pointer to the raw buffer data and modifies it.
	 */
	virtual void Modify(const ModifierFn &modifier) = 0;
};

#endif	// GELLY_IMANAGEDBUFFER_H
