#include "Shading.h"

#include <tracy/Tracy.hpp>
#include <vector>

#include "Camera.h"
#include "Rendering.h"
#include "Textures.h"

using namespace testbed;

static SSFXEffect shadingSSFXEffect;
static ssfx::ShadingCBuffer cbuffer;

void ssfx::InitializeShadingSSFX(ILogger *logger) {
	logger->Info("Creating the shading SSFX effect...");
	if (!IsSSFXInitialized()) {
		logger->Error("SSFX system is not initialized!");
	}

	shadingSSFXEffect.pixelShaderPath = "shaders/ShadingSSFX.ps50.hlsl.dxbc";
	shadingSSFXEffect.inputTextures = {
		GBUFFER_ALBEDO_TEXNAME,
		GBUFFER_NORMAL_TEXNAME,
		GBUFFER_DEPTH_TEXNAME,
		GBUFFER_POSITION_TEXNAME
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

	RegisterSSFXEffect(SHADINGSSFX_EFFECT_NAME, shadingSSFXEffect);

	cbuffer.lightCount = 2.0f;
	cbuffer.lights[0].position = float3(2.0f, 1.0f, 2.0f);
	cbuffer.lights[0].power = 0.f;
	cbuffer.lights[0].radius = 1.2f;
	cbuffer.lights[0].color = float3(1.0f, 1.0f, 1.0f);
	cbuffer.lights[1].position = float3(20, 50.f, 0);  // Nice distant light
	cbuffer.lights[1].power = 60.f;
	cbuffer.lights[1].radius = 200.f;
	cbuffer.lights[1].color = float3(1.0f, 1.0f, 1.0f);
	SetStructAsEffectConstant(&cbuffer, SHADINGSSFX_EFFECT_NAME, 1);

	const auto &renderCB = CreateGenericRenderCBuffer(GetCamera());
	SetStructAsEffectConstant(&renderCB, SHADINGSSFX_EFFECT_NAME);

	logger->Info("Done!");
}

void ssfx::UpdateShadingSSFXConstants() {
	ZoneScoped;
	static float t = 0.0f;
	t += 0.01f;
	cbuffer.lights[0].position = float3(2.0f * cosf(t), 2.0f, 2.0f * sinf(t));
	cbuffer.lights[0].color = float3(fabsf(cosf(t)), 0.3f, 1.0f);

	SetStructAsEffectConstant(&cbuffer, SHADINGSSFX_EFFECT_NAME, 1);

	const auto &renderCB = CreateGenericRenderCBuffer(GetCamera());
	SetStructAsEffectConstant(&renderCB, SHADINGSSFX_EFFECT_NAME);
}