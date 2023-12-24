#include "Camera.h"

#include <tracy/Tracy.hpp>

#include "Logging.h"
#include "SDL.h"
#include "Window.h"

using namespace DirectX;
using namespace testbed;

static Camera camera;
static bool cameraEnabled = true;
static ILogger *logger = nullptr;

void CameraEventInterceptor(SDL_Event *event) {
	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
			case SDLK_ESCAPE:
				cameraEnabled = !cameraEnabled;
				SDL_SetRelativeMouseMode(cameraEnabled ? SDL_TRUE : SDL_FALSE);
				break;
			default:
				break;
		}
	}

	if (!cameraEnabled) {
		return;
	}

	XMVECTOR forward = XMLoadFloat3(&camera.dir);
	XMVECTOR worldUp = XMLoadFloat3(&UP_VECTOR);
	XMVECTOR right = XMVector3Cross(forward, worldUp);
	XMVECTOR up = XMVector3Cross(right, forward);

	XMVECTOR rotation = XMVectorZero();

	float pitch;
	float yaw;
	switch (event->type) {
		case SDL_MOUSEMOTION:
			pitch = static_cast<float>(event->motion.xrel) / 1000.f;
			yaw = static_cast<float>(event->motion.yrel) / 1000.f;

			rotation = XMVectorAdd(rotation, XMVectorScale(right, pitch));
			rotation = XMVectorAdd(rotation, XMVectorScale(up, -yaw));

			break;
		case SDL_MOUSEWHEEL:
			camera.fov -= event->wheel.y;
			camera.fov = fminf(fmaxf(camera.fov, 1.f), 179.f);
			break;
		default:
			break;
	}

	// add rotation to forward
	forward = XMVector3Normalize(XMVectorAdd(forward, rotation));
	XMStoreFloat3(&camera.dir, forward);
}

void testbed::UpdateCamera() {
	ZoneScoped;
	if (!cameraEnabled) {
		return;
	}

	// We have to do movement here for smooth movement
	auto eye = XMLoadFloat3(&camera.position);
	auto forward = XMLoadFloat3(&camera.dir);
	auto worldUp = XMLoadFloat3(&UP_VECTOR);
	auto right = XMVector3Cross(forward, worldUp);
	auto movement = XMVectorZero();

	const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);

	if (keyboardState[SDL_SCANCODE_W]) {
		movement = XMVectorAdd(movement, forward);
	}
	if (keyboardState[SDL_SCANCODE_S]) {
		movement = XMVectorSubtract(movement, forward);
	}
	if (keyboardState[SDL_SCANCODE_A]) {
		movement = XMVectorSubtract(movement, right);
	}
	if (keyboardState[SDL_SCANCODE_D]) {
		movement = XMVectorAdd(movement, right);
	}

	const bool is_fast = keyboardState[SDL_SCANCODE_LSHIFT];
	const bool is_slow = keyboardState[SDL_SCANCODE_LCTRL];

	movement =
		XMVectorScale(movement, is_fast ? 0.1f : (is_slow ? 0.01f : 0.05f));

	XMStoreFloat3(&camera.position, XMVectorAdd(eye, movement));
}

void testbed::InitializeCamera(ILogger *newLogger) {
	logger = newLogger;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	camera.fov = 90.f;
	camera.aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
	camera.nearPlane = 0.1f;
	camera.farPlane = 1000.f;

	camera.dir = {0.0, 0.0, 1.0};
	logger->Info("Hooking up camera control interceptor");

	AddEventInterceptor(CameraEventInterceptor);
}

const Camera &testbed::GetCamera() { return camera; }