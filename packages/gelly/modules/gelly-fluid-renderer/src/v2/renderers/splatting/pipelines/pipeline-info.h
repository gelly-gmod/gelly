#ifndef PIPELINE_INFO_H
#define PIPELINE_INFO_H
#include <array>
#include <memory>

#include "device.h"
#include "renderers/frames-in-flight.h"
#include "renderers/splatting/buffers.h"
#include "renderers/splatting/texture-registry.h"

namespace gelly {
namespace renderer {
namespace splatting {
struct PipelineInfo {
	static constexpr auto MAX_FRAMES = splatting::MAX_FRAMES;

	const std::shared_ptr<Device> device;
	std::array<std::shared_ptr<InternalTextures>, MAX_FRAMES> internalTextures;
	std::array<std::shared_ptr<OutputTextures>, MAX_FRAMES> outputTextures;
	std::array<std::shared_ptr<InternalBuffers>, MAX_FRAMES> internalBuffers;
	unsigned int width;
	unsigned int height;
};
}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// PIPELINE_INFO_H
