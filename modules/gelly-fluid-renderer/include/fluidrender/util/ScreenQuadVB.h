#ifndef SCREENQUADVB_H
#define SCREENQUADVB_H

#include "fluidrender/IManagedBuffer.h"
#include "fluidrender/IManagedBufferLayout.h"
#include "fluidrender/IRenderContext.h"

namespace Gelly::util {
using ScreenQuadTuple = std::tuple<
	GellyInterfaceVal<IManagedBuffer>,
	GellyInterfaceVal<IManagedBufferLayout>>;

inline ScreenQuadTuple GenerateScreenQuad(
	GellyInterfaceRef<IRenderContext> context,
	GellyInterfaceRef<IManagedShader> vertexShader
) {
	// We just pipe 4 verts to a vertex buffer, and then we draw them as a
	// triangle strip. Of course, we also add texture coordinates.

	constexpr float quadVerts[] = {
		-1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f,  // Top left
		1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f,  // Top right
		-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // Bottom left
		1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, 1.0f   // Bottom right
	};

	BufferDesc desc{};
	desc.usage = BufferUsage::DEFAULT;
	desc.type = BufferType::VERTEX;
	// not exactly correct - but the format is not used anyway
	desc.format = BufferFormat::R32G32B32A32_FLOAT;
	desc.stride = sizeof(float) * 6;
	desc.byteWidth = sizeof(quadVerts);
	desc.initialData = static_cast<const void *>(quadVerts);

	auto *buffer = context->CreateBuffer(desc);

	BufferLayoutDesc layoutDesc{};
	layoutDesc.items[0] = {0, "SV_POSITION", 0, BufferLayoutFormat::FLOAT4};
	layoutDesc.items[1] = {0, "TEXCOORD", 0, BufferLayoutFormat::FLOAT2};
	layoutDesc.itemCount = 2;
	layoutDesc.topology = BufferLayoutTopology::TRIANGLE_STRIP;
	layoutDesc.vertexShader = vertexShader;

	auto *layout = context->CreateBufferLayout(layoutDesc);

	return std::make_tuple(buffer, layout);
}

}  // namespace Gelly::util

#endif	// SCREENQUADVB_H
