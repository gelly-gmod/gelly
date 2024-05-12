#ifndef COMPOSITE_H
#define COMPOSITE_H
#include "Pipeline.h"
#include "PipelineType.h"
#include "Resources.h"

class GModCompositor {
private:
	Pipeline *pipeline;
	Resources resources;
	GellyResources gellyResources;

public:
	GModCompositor(
		PipelineType type,
		GellyInterfaceVal<IFluidRenderer> renderer,
		GellyInterfaceVal<IRenderContext> context
	);

	void SetConfig(PipelineConfig config);
	void Render();
};

#endif	// COMPOSITE_H
