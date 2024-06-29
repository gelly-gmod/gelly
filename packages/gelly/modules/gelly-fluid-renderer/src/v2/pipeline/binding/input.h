#ifndef INPUT_H
#define INPUT_H
#include <memory>
#include <variant>

#include "resources/buffer.h"
#include "resources/input-layout.h"
#include "resources/texture.h"

namespace gelly::renderer {
struct InputBuffer {
	const std::shared_ptr<Buffer> buffer;
	D3D11_BIND_FLAG bindFlag;
	unsigned int slot;
};

struct InputVertexBuffer {
	const std::shared_ptr<Buffer> vertexBuffer;
	unsigned int slot;
};

struct InputTexture {
	const std::shared_ptr<Texture> texture;
	D3D11_BIND_FLAG bindFlag;
	unsigned int slot;
};

using Input = std::variant<InputBuffer, InputVertexBuffer, InputTexture>;
}  // namespace gelly::renderer

#endif	// INPUT_H
