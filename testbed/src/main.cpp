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
#include "Window.h"
#include "ssfx\Composite.h"
#include "ssfx\Shading.h"

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
	ssfx::InitializeCompositeSSFX(logger);

	LoadScene({"assets/01_gelly_springs.gltf"});
	bool isRunning = true;
	while (isRunning) {
		isRunning = HandleWindowMessages();

		UpdateCamera();
		StartFrame();
		RenderScene();
		ssfx::UpdateShadingSSFXConstants();
		ApplySSFXEffect(SHADINGSSFX_EFFECT_NAME);
		ApplySSFXEffect(COMPOSITESSFX_EFFECT_NAME);
		EndFrame();
		FrameMark;

#ifdef _DEBUG
		LogRenderDebugMessages(debugLayer);
#endif
	}

	return 0;
}
