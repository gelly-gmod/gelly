#include <cstdio>

#include "Camera.h"
#include "Logging.h"
#include "Rendering.h"
#include "SSFX.h"
#include "Scene.h"
#include "Shaders.h"
#include "Textures.h"
#include "Window.h"

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
	InitializeTextureSystem(logger, rendererDevice);
	InitializeCamera(logger);
	InitializeShaderSystem(logger);
	InitializeSceneSystem(logger);
	InitializeSSFXSystem(logger, rendererDevice);

	LoadScene({"assets/test_sphere.gltf"});
	bool isRunning = true;
	while (isRunning) {
		isRunning = HandleWindowMessages();

		UpdateCamera();
		StartFrame();
		RenderScene();
		EndFrame();

#ifdef _DEBUG
		LogRenderDebugMessages(debugLayer);
#endif
	}

	return 0;
}
