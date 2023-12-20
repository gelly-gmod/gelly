#ifndef GELLY_H
#define GELLY_H

#include <GellyFluidSim.h>

#include <variant>

#include "ILogger.h"
#include "Memory.h"
#include "fluidrender/IFluidRenderer.h"

#define GELLY_ALBEDO_TEXNAME "gelly/albedo"
#define GELLY_DEPTH_TEXNAME "gelly/depth"
#define GELLY_NORMAL_TEXNAME "gelly/normals"
#define GELLY_POSITIONS_TEXNAME "gelly/positions"
#define GELLY_THICKNESS_TEXNAME "gelly/thickness"

namespace testbed {
/**
 * \brief Marks what type of simulation is being actively used, you may also use
 * this enum to determine which downcasts are safe to perform.
 */
enum class GellySimMode {
	DEBUG,
	RTFR,
};

/**
 * \brief Struct for initialization info for the Gelly simulation.
 * The variant needs to match the GellySimMode enum.
 */
struct GellySimInit {
	struct RTFRInfo {
		std::filesystem::path folderPath;
	};

	struct DebugInfo {
		int maxParticles = 10;
	};

	GellySimMode mode = GellySimMode::DEBUG;
	std::variant<std::monostate, RTFRInfo, DebugInfo> modeInfo;
};

void InitializeGelly(ID3D11Device *rendererDevice, ILogger *newLogger);
GellySimMode GetCurrentGellySimMode();
void InitializeNewGellySim(const GellySimInit &init);

IFluidSimulation *GetGellyFluidSim();
IFluidRenderer *GetGellyFluidRenderer();
IRenderContext *GetGellyRenderContext();
Gelly::FluidRenderSettings GetGellyFluidRenderSettings();
void UpdateGellyFluidRenderSettings(const Gelly::FluidRenderSettings &settings);
}  // namespace testbed
#endif	// GELLY_H
