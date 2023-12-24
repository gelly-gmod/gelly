#include "ShadingWater.h"

#include <tracy/Tracy.hpp>
#include <vector>

#include "Camera.h"
#include "Rendering.h"
#include "Shading.h"
#include "Textures.h"

using namespace testbed;

static SSFXEffect shadingSSFXEffect;

void ssfx::InitializeShadingWaterSSFX(ILogger *logger) {
	logger->Info("Creating the shading water SSFX effect...");
	if (!IsSSFXInitialized()) {
		logger->Error("SSFX system is not initialized!");
	}

	shadingSSFXEffect.pixelShaderPath =
		"shaders/ShadingWaterSSFX.ps50.hlsl.dxbc";
	shadingSSFXEffect.inputTextures = {
		GBUFFER_ALBEDO_TEXNAME,
		GBUFFER_NORMAL_TEXNAME,
		GBUFFER_DEPTH_TEXNAME,
		GBUFFER_POSITION_TEXNAME,
		BUILTIN_BACKBUFFER_OPAQUE_TEXNAME
	};

	shadingSSFXEffect.outputTextures = {BUILTIN_BACKBUFFER_TEXNAME};
	shadingSSFXEffect.shaderConstantData[0] =
		std::make_shared<std::vector<unsigned char>>();
	shadingSSFXEffect.shaderConstantData[0]->resize(sizeof(GenericRenderCBuffer)
	);

	shadingSSFXEffect.shaderConstantData[1] =
		std::make_shared<std::vector<unsigned char>>();
	shadingSSFXEffect.shaderConstantData[1]->resize(sizeof(ShadingCBuffer));

	shadingSSFXEffect.shaderConstantBufferCount = 2;

	RegisterSSFXEffect(SHADINGWATERSSFX_EFFECT_NAME, shadingSSFXEffect);

	const auto cbuffer = GetShadingSSFXCBuffer();
	SetStructAsEffectConstant(&cbuffer, SHADINGWATERSSFX_EFFECT_NAME, 1);

	const auto &renderCB = CreateGenericRenderCBuffer(GetCamera());
	SetStructAsEffectConstant(&renderCB, SHADINGWATERSSFX_EFFECT_NAME);

	logger->Info("Done!");
}

void ssfx::UpdateShadingWaterSSFXConstants() {
	ZoneScoped;
	const auto cbuffer = GetShadingSSFXCBuffer();
	SetStructAsEffectConstant(&cbuffer, SHADINGWATERSSFX_EFFECT_NAME, 1);

	const auto &renderCB = CreateGenericRenderCBuffer(GetCamera());
	SetStructAsEffectConstant(&renderCB, SHADINGWATERSSFX_EFFECT_NAME);
}