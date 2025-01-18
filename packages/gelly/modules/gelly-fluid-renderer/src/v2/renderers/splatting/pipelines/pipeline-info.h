#ifndef PIPELINE_INFO_H
#define PIPELINE_INFO_H
#include <array>
#include <memory>

#include "device.h"
#include "renderers/splatting/buffers.h"
#include "renderers/splatting/texture-registry.h"

namespace gelly {
namespace renderer {
namespace splatting {
struct PipelineInfo {
	// basically double buffering
	static constexpr auto MAX_FRAMES = 2;

	const std::shared_ptr<Device> device;
	std::array<std::shared_ptr<InternalTextures>, MAX_FRAMES> internalTextures;
	std::array<std::shared_ptr<OutputTextures>, MAX_FRAMES> outputTextures;
	const std::shared_ptr<InternalBuffers> internalBuffers;
	unsigned int width;
	unsigned int height;
};
}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// PIPELINE_INFO_H
