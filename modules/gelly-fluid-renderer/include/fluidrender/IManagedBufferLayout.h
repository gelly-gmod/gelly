#ifndef IMANAGEDBUFFERLAYOUT_H
#define IMANAGEDBUFFERLAYOUT_H

#include <GellyInterface.h>

#include <cstdint>

#include "IManagedBuffer.h"

namespace Gelly {
enum class BufferLayoutFormat : uint8_t {
	FLOAT2,
	FLOAT4,
};

struct BufferLayoutItem {
	/**
	 * \brief Main purpose of this is for matrices.
	 * If you want to, for example, describe a 4x4 matrix,
	 * you'd have four entries with the same semantic name,
	 * and the semantic index would be the row index.
	 */
	uint8_t semanticIndex;
	const char *semanticName;
	uint8_t slotIndex;
	BufferLayoutFormat format;
};

struct BufferLayoutDesc {
	BufferLayoutItem items[8];
	uint8_t itemCount;
	GellyObserverPtr<IManagedShader> vertexShader;
};
}  // namespace Gelly

/**
 * \brief A buffer layout is a description of how a buffer is laid out as input
 * to a vertex shader. It is not for anything else, so this can be skipped if
 * you're not planning to use a buffer with a vertex shader as input.
 */
gelly_interface IManagedBufferLayout {
public:
	virtual ~IManagedBufferLayout() = default;

	virtual const Gelly::BufferLayoutDesc &GetLayoutDesc() = 0;
	virtual void SetLayoutDesc(const Gelly::BufferLayoutDesc &desc) = 0;

	virtual void AttachToContext(GellyObserverPtr<IRenderContext> context) = 0;

	virtual void Create() = 0;
	virtual void Destroy() = 0;

	virtual void AttachBufferAtSlot(GellyObserverPtr<IManagedBuffer> buffer, uint8_t slot) = 0;

	virtual void BindAsVertexBuffer() = 0;
};

#endif	// IMANAGEDBUFFERLAYOUT_H
