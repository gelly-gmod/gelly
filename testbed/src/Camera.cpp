#include "Camera.h"

#include "Logging.h"
#include "SDL.h"
#include "Window.h"

using namespace DirectX;
using namespace testbed;

static Camera camera;

void MovementInterceptor(SDL_Event *event) {
	// Right now the camera just functions as an FPS camera
	// We do need to calculate the right vector and up vector
	// as the camera just operates based off of an eye position and
	// a forward vector
	// so we filter events to reduce the amount of work we have to do

	XMVECTOR eye = XMLoadFloat3(&camera.position);
	XMVECTOR forward = XMLoadFloat3(&camera.dir);
	XMVECTOR worldUp = XMLoadFloat3(&UP_VECTOR);
	XMVECTOR right = XMVector3Cross(forward, worldUp);
	XMVECTOR up = XMVector3Cross(right, forward);

	XMVECTOR movement = XMVectorZero();
	XMVECTOR rotation = XMVectorZero();

	float pitch;
	float yaw;
	switch (event->type) {
		case SDL_MOUSEMOTION:
			pitch = (float)event->motion.xrel / 1000.f;
			yaw = (float)event->motion.yrel / 1000.f;

			rotation = XMVectorAdd(rotation, XMVectorScale(right, -pitch));
			rotation = XMVectorAdd(rotation, XMVectorScale(up, -yaw));

			break;
		default:
			break;
	}

	// add rotation to forward
	forward = XMVector3Normalize(XMVectorAdd(forward, rotation));

	// For movement, we get the keyboard state so that it applies
	// as long as the key is held down
	const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);

	if (keyboardState[SDL_SCANCODE_W]) {
		movement = XMVectorAdd(movement, forward);
	}
	if (keyboardState[SDL_SCANCODE_S]) {
		movement = XMVectorSubtract(movement, forward);
	}
	if (keyboardState[SDL_SCANCODE_A]) {
		movement = XMVectorAdd(movement, right);
	}
	if (keyboardState[SDL_SCANCODE_D]) {
		movement = XMVectorSubtract(movement, right);
	}

	movement = XMVectorScale(movement, 0.04f);

	XMStoreFloat3(&camera.dir, forward);
	XMStoreFloat3(&camera.position, XMVectorAdd(eye, movement));
}

void testbed::InitializeCamera() {
	SDL_SetRelativeMouseMode(SDL_TRUE);

	camera.fov = 75.f;
	camera.aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
	camera.nearPlane = 0.1f;
	camera.farPlane = 1000.f;

	camera.dir = {0.0, 0.0, 1.0};

	GetLogger()->Info("Hooking up camera control interceptor");

	AddEventInterceptor(MovementInterceptor);
}

const Camera &testbed::GetCamera() { return camera; }