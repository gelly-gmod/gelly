#ifndef RELOAD_SHADERS_H
#define RELOAD_SHADERS_H

#ifndef GELLY_ENABLE_RENDERDOC_CAPTURES
#error \
	"You are not compiling with a RenderDoc capture enabled build. Please switch to a profile that has RenderDoc enabled in order to enable hot reloading."
#endif
#include "AlbedoDownsamplePS.h"
#include "EstimateNormalPS.h"
#include "FilterDepthPS.h"
#include "ScreenQuadVS.h"
#include "SplattingGS.h"
#include "SplattingPS.h"
#include "SplattingVS.h"

namespace gelly::renderer::splatting {
inline auto ReloadAllGSCShaders() -> void {
	gsc::SplattingVS::ReloadFromDisk();
	gsc::SplattingGS::ReloadFromDisk();
	gsc::SplattingPS::ReloadFromDisk();
	gsc::AlbedoDownsamplePS::ReloadFromDisk();
	gsc::EstimateNormalPS::ReloadFromDisk();
	gsc::ScreenQuadVS::ReloadFromDisk();
	gsc::FilterDepthPS::ReloadFromDisk();
}
}  // namespace gelly::renderer::splatting
#endif	// RELOAD_SHADERS_H
