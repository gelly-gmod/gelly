#ifndef COMPOSITE_H
#define COMPOSITE_H
#include "Pipeline.h"
#include "PipelineType.h"
#include "Resources.h"

class GModCompositor {
private:
	Pipeline *pipeline;
	GellyResources gellyResources;

public:
	GModCompositor(
		PipelineType type,
		std::shared_ptr<IFluidRenderer> renderer,
		std::shared_ptr<IRenderContext> context
	);

	void SetConfig(PipelineConfig config);
	[[nodiscard]] PipelineConfig GetConfig() const;
	void SetFluidMaterial(const PipelineFluidMaterial &material);

	void Render();
};

#endif	// COMPOSITE_H
