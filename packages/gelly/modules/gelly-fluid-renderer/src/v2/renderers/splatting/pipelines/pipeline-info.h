#ifndef PIPELINE_INFO_H
#define PIPELINE_INFO_H
#include <memory>

#include "device.h"
#include "renderers/splatting/buffers.h"
#include "renderers/splatting/texture-registry.h"

namespace gelly {
namespace renderer {
namespace splatting {
struct PipelineInfo {
	const std::shared_ptr<Device> device;
	const std::shared_ptr<InternalTextures> internalTextures;
	const std::shared_ptr<OutputTextures> outputTextures;
	const std::shared_ptr<InternalBuffers> internalBuffers;
	const unsigned int width;
	const unsigned int height;
};
}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// PIPELINE_INFO_H
