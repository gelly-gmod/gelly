#include <GellyRenderer.h>

#include <cstdio>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "BasicD3D9Renderer.h"

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
		"Gelly Test D3D9Ex Renderer",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		840,
		640,
		0
	);

	if (window == nullptr) {
		printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
		return 1;
	}

	BasicD3D9Renderer d3d9Renderer(window);

	RendererInitParams gellyParams = {
		.maxParticles = 32,
		.width = 840,
		.height = 640,
		.sharedTextures = {
			.depth = d3d9Renderer.GetInputDepthSharedHandle(),
			.normal = d3d9Renderer.GetInputNormalSharedHandle()}};

	GellyRenderer *renderer = GellyRenderer_Create(gellyParams);

	bool quit = false;
	float yaw = 0.f;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) quit = true;
		}

		yaw += 0.01f;
		renderer->camera.SetDirection(sinf(yaw), 0.f, cosf(yaw));
		renderer->SetActiveParticles(31);
		renderer->Render();
		d3d9Renderer.Render();
#ifdef _DEBUG
		renderer->PrintDebugMessages();
#endif
		// Sleep for 1 / 60
		SDL_Delay(1000 / 60);
	}

	GellyRenderer_Destroy(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
