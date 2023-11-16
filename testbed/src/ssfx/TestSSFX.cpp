#include "TestSSFX.h"

#include "Textures.h"

using namespace testbed;

static SSFXEffect testSSFXEffect = {
	.pixelShaderPath = "shaders/TestSSFX.ps50.hlsl.dxbc",
	.inputTextures = {BUILTIN_BACKBUFFER_TEXNAME},
	.outputTextures = {BUILTIN_BACKBUFFER_TEXNAME},

	.shaderConstantData = nullptr
};

void ssfx::InitializeTestSSFX(ILogger *logger) {
	logger->Info("Creating the test SSFX effect...");
	if (!IsSSFXInitialized()) {
		logger->Error("SSFX system is not initialized!");
	}
	RegisterSSFXEffect(TESTSSFX_EFFECT_NAME, testSSFXEffect);
	logger->Info("Done!");
}