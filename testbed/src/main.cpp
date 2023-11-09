#include <cstdio>

#include "Camera.h"
#include "Logging.h"
#include "Rendering.h"
#include "Scene.h"
#include "Window.h"

using namespace testbed;

int main() {
	InitializeLogger(LoggerType::Console);
	GetLogger()->Info("Hello, world!");

	InitializeSDL();
	MakeTestbedWindow();
	InitializeRenderer();
	InitializeCamera();
	LoadScene({"assets/test_sphere.gltf"});
	bool isRunning = true;
	while (isRunning) {
		isRunning = HandleWindowMessages();

		StartFrame();
		RenderScene();
		EndFrame();
	}

	return 0;
}
