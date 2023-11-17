#include "Shading.h"

#include "Rendering.h"
#include "Textures.h"

using namespace testbed;

static SSFXEffect shadingSSFXEffect;

void ssfx::InitializeShadingSSFX(ILogger *logger) {
	logger->Info("Creating the test SSFX effect...");
	if (!IsSSFXInitialized()) {
		logger->Error("SSFX system is not initialized!");
	}

	shadingSSFXEffect.pixelShaderPath = "shaders/ShadingSSFX.ps50.hlsl.dxbc";
	shadingSSFXEffect.inputTextures = {
		GBUFFER_ALBEDO_TEXNAME,
		GBUFFER_NORMAL_TEXNAME,
		GBUFFER_DEPTH_TEXNAME,
	};
	shadingSSFXEffect.outputTextures = {BUILTIN_BACKBUFFER_TEXNAME};
	shadingSSFXEffect.shaderConstantData = nullptr;

	RegisterSSFXEffect(SHADINGSSFX_EFFECT_NAME, shadingSSFXEffect);
	logger->Info("Done!");
}