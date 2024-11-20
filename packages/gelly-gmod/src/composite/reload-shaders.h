#ifndef GMOD_RELOAD_SHADERS_H
#define GMOD_RELOAD_SHADERS_H
#include "CompositeBackbufferPS.h"
#include "CompositeFoamPS.h"
#include "CompositePS.h"
#include "NDCQuadVS.h"
#include "OutputCompositePS.h"

#ifndef GELLY_ENABLE_RENDERDOC_CAPTURES
#error \
	"You are not compiling with a RenderDoc capture enabled build. Please switch to a profile that has RenderDoc enabled in order to enable hot reloading."
#endif

namespace gelly::gmod::renderer {
inline auto ReloadAllGSCShaders() -> void {
	gsc::CompositePS::ReloadFromDisk();
	gsc::CompositeFoamPS::ReloadFromDisk();
	gsc::NDCQuadVS::ReloadFromDisk();
	gsc::OutputCompositePS::ReloadFromDisk();
	gsc::CompositeBackbufferPS::ReloadFromDisk();
}
}  // namespace gelly::gmod::renderer
#endif	// GMOD_RELOAD_SHADERS_H
