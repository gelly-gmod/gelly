#include "Gelly.h"

#include <GMFS.h>
#include <d3d9.h>

#include "source/D3DDeviceWrapper.h"

// Doesn't work on the main thread.
Gelly::Gelly(GellyInitParams &params)
	: scene(nullptr),
	  renderer(nullptr),
	  engine(nullptr),
	  compositor(
		  GetD3DDevice(), params.width, params.height, &params.sharedTextures
	  ),
	  mainToThread(0),
	  threadToMain(0),
	  thread(&Gelly::InitThreaded, this, params),
	  message(nullptr) {
	thread.detach();
}

Gelly::~Gelly() {
	GellyRenderer_Destroy(renderer);
	GellyEngine_DestroyScene(scene);
}

void Gelly::LoadMap(const char *mapName) {
	char fullMapPath[256];
	sprintf_s(fullMapPath, "maps/%s.bsp", mapName);

	if (!FileSystem::Exists(fullMapPath, "GAME")) {
		return;
	}

	FileHandle_t file = FileSystem::Open(fullMapPath, "rb");
	uint32_t size = FileSystem::Size(file);
	auto *buffer = static_cast<uint8_t *>(malloc(size));
	FileSystem::Read(buffer, FileSystem::Size(file), file);
	scene->EnterGPUWork();
	scene->AddBSP(mapName, buffer, size);
	scene->colliders.Update();
	scene->ExitGPUWork();
	free(buffer);
	FileSystem::Close(file);
}

void Gelly::Update(float deltaTime) {
	scene->EnterGPUWork();
	scene->colliders.Update();
	scene->ExitGPUWork();
	scene->Update(deltaTime);
}

void Gelly::Render() {
	renderer->SetActiveParticles(scene->GetCurrentParticleCount());
	renderer->Render();
}

void Gelly::AddParticle(Vec4 position, Vec3 velocity) {
	scene->EnterGPUWork();
	scene->AddParticle(position, velocity);
	scene->ExitGPUWork();
};

void Gelly::SetupCamera(
	float fov, float width, float height, float nearZ, float farZ
) {
	renderer->camera.SetPerspective(fov, width, height, nearZ, farZ);
}

void Gelly::SyncCamera(Vec3 position, Vec3 dir) {
	renderer->camera.SetPosition(position.x, position.y, position.z);
	renderer->camera.SetDirection(dir.x, dir.y, dir.z);
}

void Gelly::Clear() { scene->Clear(); }

void Gelly::SetParticleRadius(float radius) {
	// TODO: Make this also change the FleX particle radius.
	scene->params->radius = radius;
	renderer->SetParticleRadius(radius * 2.f);
}

[[noreturn]] void Gelly::InitThreaded(
	Gelly *gelly, const GellyInitParams &params
) {
	RendererInitParams rendererParams{
		.maxParticles = params.maxParticles,
		.width = params.width,
		.height = params.height,
		.sharedTextures = params.sharedTextures};

	gelly->renderer = GellyRenderer_Create(rendererParams);
	gelly->scene = GellyEngine_CreateScene(
		params.maxParticles,
		params.maxColliders,
		gelly->renderer->GetD3DDevice()
	);
	
	gelly->engine = new GellyEngineGMod(
		params.maxParticles,
		params.maxColliders,
		gelly->renderer->GetD3DDevice()
	);

	gelly->scene->RegisterD3DBuffer(
		gelly->renderer->GetD3DParticleBuffer(),
		params.maxParticles,
		sizeof(Vec4)
	);

	while (true) {
		// Wait for a signal
		gelly->mainToThread.acquire();
		// Process the message
		gelly->ProcessMessage();
		// Signal the main thread that we're done
		gelly->threadToMain.release();
	}
}

void Gelly::SendGellyMessage(GellyMessage &newMessage) {
	message = &newMessage;
	mainToThread.release();
	threadToMain.acquire();
	message = nullptr;
}

void Gelly::ProcessMessage() {
	switch (message->type) {
		case GellyMessage::LoadMap:
			LoadMap(message->loadMap.mapName);
			break;
		case GellyMessage::Update:
			Update(message->update.deltaTime);
			break;
		case GellyMessage::Render:
			Render();
			break;
		case GellyMessage::AddParticle:
			AddParticle(
				message->addParticle.position, message->addParticle.velocity
			);
			break;
		case GellyMessage::SetupCamera:
			SetupCamera(
				message->setupCamera.fov,
				message->setupCamera.width,
				message->setupCamera.height,
				message->setupCamera.nearZ,
				message->setupCamera.farZ
			);
			break;
		case GellyMessage::SyncCamera:
			SyncCamera(
				message->syncCamera.position, message->syncCamera.direction
			);
			break;
		case GellyMessage::Clear:
			Clear();
			break;
		case GellyMessage::SetParticleRadius:
			SetParticleRadius(message->setParticleRadius.radius);
			break;
		default:
			break;
	}
}