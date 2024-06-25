#ifndef COMPOSITE_H
#define COMPOSITE_H
#include "Pipeline.h"
#include "PipelineType.h"
#include "Resources.h"
#include "logging/global-macros.h"

class GModCompositor {
private:
	std::unique_ptr<Pipeline> pipeline;
	GellyResources gellyResources;

public:
	GModCompositor(PipelineType type, const std::shared_ptr<ISimData> &simData);

	GModCompositor(const GModCompositor &) = delete;
	GModCompositor &operator=(const GModCompositor &) = delete;
	GModCompositor(GModCompositor &&) = delete;
	GModCompositor &operator=(GModCompositor &&) = delete;

	~GModCompositor() {
		LOG_INFO("GModCompositor destructor called");
		// we dont need to delete pipeline because it is a unique_ptr
	}

	void SetConfig(PipelineConfig config);
	[[nodiscard]] PipelineConfig GetConfig() const;
	void SetFluidMaterial(const PipelineFluidMaterial &material);

	void Composite();
	void Render();
};

#endif	// COMPOSITE_H
