#include "Composite.h"

#include "Gelly.h"
#include "Textures.h"

using namespace testbed;

static ILogger *logger = nullptr;
static SSFXEffect compositeSSFXEffect;

void testbed::ssfx::InitializeCompositeSSFX(ILogger *logger) {
	logger->Info("Creating the gelly composite SSFX effect...");
	if (!IsSSFXInitialized()) {
		logger->Error("SSFX system is not initialized!");
	}

	compositeSSFXEffect.pixelShaderPath =
		"shaders/CompositeSSFX.ps50.hlsl.dxbc";
	compositeSSFXEffect.inputTextures = {
		GELLY_DEPTH_TEXNAME,
		GELLY_NORMAL_TEXNAME,
		GELLY_ALBEDO_TEXNAME,
		GELLY_POSITIONS_TEXNAME,
		GELLY_THICKNESS_TEXNAME
	};

	compositeSSFXEffect.outputTextures = {
		GBUFFER_NORMAL_TEXNAME,
		GBUFFER_ALBEDO_TEXNAME,
		GBUFFER_DEPTH_TEXNAME,
		GBUFFER_POSITION_TEXNAME
	};

	RegisterSSFXEffect(COMPOSITESSFX_EFFECT_NAME, compositeSSFXEffect);

	logger->Info("Done!");
}