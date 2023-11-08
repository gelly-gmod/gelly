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

	GetLogger()->Info("Handling event");
	switch (event->type) {
		case SDL_MOUSEMOTION:
			rotation = XMVectorSet(
				(float)event->motion.xrel, (float)event->motion.yrel, 0.0f, 0.0f
			);
			break;
		default:
			break;
	}

	// add rotation to forward
	forward = XMVector3Normalize(XMVectorAdd(forward, rotation));

	XMStoreFloat3(&camera.dir, forward);
}

void testbed::InitializeCamera() {
	camera.fov = 75.f;
	camera.aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
	camera.nearPlane = 0.1f;
	camera.farPlane = 1000.f;

	GetLogger()->Info("Hooking up camera control interceptor");

	AddEventInterceptor(MovementInterceptor);
}

const Camera &testbed::GetCamera() { return camera; }