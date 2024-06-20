#ifndef VALIDATE_PIPELINE_H
#define VALIDATE_PIPELINE_H
#include <string>

#include "pipeline.h"

namespace gelly::renderer {
struct PipelineValidationError {
	const std::string friendlyMessage;
};

/**
 * A useful debugging function to validate a pipeline by examining its create
 * info. If, for example, a constant buffer is bound and it is not actually
 * created *as* a constant buffer, this function will catch it.
 *
 * Or, maybe a texture is bound as an output, but it was not created with the
 * proper config for a render target, this function will also catch it.
 *
 * However, it's usually a bad idea to run this in any sort of production
 * environment, so this function call should only be compiled for debug builds.
 * @param createInfo The pipeline create info to validate.
 */
auto ValidatePipeline(const Pipeline::PipelineCreateInfo &createInfo)
	-> std::optional<std::vector<PipelineValidationError>>;

}  // namespace gelly::renderer

#endif	// VALIDATE_PIPELINE_H
