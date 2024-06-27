#ifndef OUTPUT_H
#define OUTPUT_H

#include <memory>
#include <variant>

#include "resources/texture.h"

namespace gelly::renderer {
struct OutputTexture {
	const std::shared_ptr<Texture> texture;
	D3D11_BIND_FLAG bindFlag;
	unsigned int slot;

	float clearColor[4];
	bool clear = true;
	bool enabled = true;
};

// As of now, it only makes sense to output to textures.
// Later on if we have compute shaders, we may easily extend this to support
// buffers.
using Output = std::variant<OutputTexture>;
}  // namespace gelly::renderer
#endif	// OUTPUT_H
