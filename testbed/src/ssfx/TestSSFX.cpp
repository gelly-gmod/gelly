#include "TestSSFX.h"

#include "Rendering.h"
#include "Textures.h"

using namespace testbed;

static SSFXEffect testSSFXEffect;

void ssfx::InitializeTestSSFX(ILogger *logger) {
	logger->Info("Creating the test SSFX effect...");
	if (!IsSSFXInitialized()) {
		logger->Error("SSFX system is not initialized!");
	}

	testSSFXEffect.pixelShaderPath = "shaders/TestSSFX.ps50.hlsl.dxbc";
	testSSFXEffect.inputTextures = {
		GBUFFER_ALBEDO_TEXNAME, GBUFFER_NORMAL_TEXNAME
	};
	testSSFXEffect.outputTextures = {BUILTIN_BACKBUFFER_TEXNAME};
	testSSFXEffect.shaderConstantData = nullptr;

	RegisterSSFXEffect(TESTSSFX_EFFECT_NAME, testSSFXEffect);
	logger->Info("Done!");
}