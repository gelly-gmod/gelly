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
	compositeSSFXEffect.inputTextures = {GELLY_DEPTH_TEXNAME};

	compositeSSFXEffect.outputTextures = {BUILTIN_BACKBUFFER_TEXNAME};
	compositeSSFXEffect.shaderConstantData = nullptr;

	RegisterSSFXEffect(COMPOSITESSFX_EFFECT_NAME, compositeSSFXEffect);

	logger->Info("Done!");
}