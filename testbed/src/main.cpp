#include <cstdio>

#include "Camera.h"
#include "Logging.h"
#include "Rendering.h"
#include "Scene.h"
#include "Shaders.h"
#include "Window.h"

using namespace testbed;

int main() {
	auto *logger = InitializeLogger(LoggerType::Console);
	logger->Info("Starting up...");

	InitializeWindow(logger);
	InitializeRenderer(logger);
	InitializeCamera(logger);
	InitializeShaderSystem(logger);
	InitializeSceneSystem(logger);
	LoadScene({"assets/Duck.gltf"});
	bool isRunning = true;
	while (isRunning) {
		isRunning = HandleWindowMessages();

		UpdateCamera();
		StartFrame();
		RenderScene();
		EndFrame();
	}

	return 0;
}
