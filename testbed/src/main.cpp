#include <cstdio>

#include "Camera.h"
#include "Logging.h"
#include "Rendering.h"
#include "SSFX.h"
#include "Scene.h"
#include "Shaders.h"
#include "Textures.h"
#include "Window.h"
#include "ssfx/TestSSFX.h"

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
	ssfx::InitializeTestSSFX(logger);

	LoadScene({"assets/test_hires_scene.gltf"});
	bool isRunning = true;
	while (isRunning) {
		isRunning = HandleWindowMessages();

		UpdateCamera();
		StartFrame();
		RenderScene();
		ApplySSFXEffect(TESTSSFX_EFFECT_NAME);
		EndFrame();

#ifdef _DEBUG
		LogRenderDebugMessages(debugLayer);
#endif
	}

	return 0;
}
