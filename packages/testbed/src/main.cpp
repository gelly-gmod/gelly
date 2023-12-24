#include <cstdio>
#include <tracy/Tracy.hpp>

#include "Camera.h"
#include "Gelly.h"
#include "Logging.h"
#include "Memory.h"
#include "Rendering.h"
#include "SSFX.h"
#include "Scene.h"
#include "Shaders.h"
#include "Textures.h"
#include "Window.h"
#include "ssfx/ShadingWater.h"
#include "ssfx/Composite.h"
#include "ssfx/Shading.h"

#ifdef _DEBUG
#include "D3D11DebugLayer.h"
#endif

using namespace testbed;

int main() {
	auto *logger = InitializeLogger(LoggerType::Console);
	logger->Info("Starting up...");

	InitializeWindow(logger);
	const auto rendererDevice = InitializeRenderer(logger);
#ifdef _DEBUG
	const auto debugLayer =
		InitializeRendererDebugLayer(logger, rendererDevice);
#endif
	InitializeCamera(logger);
	InitializeShaderSystem(logger);
	InitializeSceneSystem(logger);
	InitializeSSFXSystem(logger, rendererDevice);
	InitializeGelly(rendererDevice, logger);
	ssfx::InitializeShadingSSFX(logger);
	ssfx::InitializeShadingWaterSSFX(logger);
	ssfx::InitializeCompositeSSFX(logger);

	LoadScene({"assets/04_gelly_bowl.gltf"});
	std::chrono::high_resolution_clock::time_point lastFrameTime =
		std::chrono::high_resolution_clock::now();

	bool isRunning = true;
	while (isRunning) {
		isRunning = HandleWindowMessages();

		UpdateCamera();
		StartFrame();
		RenderScene();
		ssfx::UpdateShadingSSFXConstants();
		ssfx::UpdateShadingWaterSSFXConstants();

		ApplySSFXEffect(SHADINGSSFX_EFFECT_NAME);
		CopyTexture(
			BUILTIN_BACKBUFFER_TEXNAME, BUILTIN_BACKBUFFER_OPAQUE_TEXNAME
		);
		ApplySSFXEffect(COMPOSITESSFX_EFFECT_NAME, true);
		ApplySSFXEffect(SHADINGWATERSSFX_EFFECT_NAME);

		EndFrame();

		std::chrono::high_resolution_clock::time_point thisFrameTime =
			std::chrono::high_resolution_clock::now();

		{
			ZoneScopedN("Fluid update");
			const float deltaTime =
				std::chrono::duration<float>(thisFrameTime - lastFrameTime)
					.count();
			GetGellyFluidSim()->Update(deltaTime);

			lastFrameTime = thisFrameTime;
		}
		FrameMark;

#ifdef _DEBUG
		LogRenderDebugMessages(debugLayer);
#endif
	}

	return 0;
}
