#include "Gelly.h"

#include <GMFS.h>
#include <d3d9.h>

#include "d3d9/D3D9Shader.h"

// Doesn't work on the main thraed.
Gelly::Gelly(
	const GellyInitParams &params,
	IDirect3DDevice9 *device,
	IDirect3DTexture9 *depthTexture
)
	: scene(nullptr),
	  renderer(nullptr),
	  compositor(device, depthTexture),
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
		default:
			break;
	}
}

void RendererCompositor::CreateScreenQuad() {
	device->CreateVertexBuffer(
		6 * sizeof(NDCVertex),
		0,
		NDCVertex::FVF,
		D3DPOOL_DEFAULT,
		screenQuad.GetAddressOf(),
		nullptr
	);

	NDCVertex *vertices;
	screenQuad->Lock(0, 0, reinterpret_cast<void **>(&vertices), 0);

	vertices[0] = {-1.f, -1.f, 0.f, 1.f, 0.f, 1.f};
	vertices[1] = {-1.f, 1.f, 0.f, 1.f, 0.f, 0.f};
	vertices[2] = {1.f, -1.f, 0.f, 1.f, 1.f, 1.f};
	vertices[3] = {1.f, -1.f, 0.f, 1.f, 1.f, 1.f};
	vertices[4] = {-1.f, 1.f, 0.f, 1.f, 0.f, 0.f};
	vertices[5] = {1.f, 1.f, 0.f, 1.f, 1.f, 0.f};

	screenQuad->Unlock();
}

const char *COMPOSITE_PS_SOURCE =
#include "generated/CompositePS.embed.hlsl"
	;

const char *COMPOSITE_VS_SOURCE =
#include "generated/CompositeVS.embed.hlsl"
	;

#define INIT_OPTIONS_FOR_SHADER(source, shaderName, shaderEntryPoint) \
	options.shader.buffer = (void *)source;                           \
	options.shader.size = strlen(source);                             \
	options.shader.name = shaderName;                                 \
	options.shader.entryPoint = shaderEntryPoint;

void RendererCompositor::CreateShaders() {
	d3d9::ShaderCompileOptions options = {
		.device = device,
		.shader = {},
		.defines = nullptr,
	};

	INIT_OPTIONS_FOR_SHADER(COMPOSITE_PS_SOURCE, "CompositePS.hlsl", "main");
	auto pixelShaderResult = d3d9::compile_pixel_shader(options);
	// .Attach has to be used to prevent releasing the underlying resource.
	pixelShader.Attach(pixelShaderResult);

	INIT_OPTIONS_FOR_SHADER(COMPOSITE_VS_SOURCE, "CompositeVS.hlsl", "main");
	auto vertexShaderResult = d3d9::compile_vertex_shader(options);
	vertexShader.Attach(vertexShaderResult);
}

RendererCompositor::RendererCompositor(
	IDirect3DDevice9 *device, IDirect3DTexture9 *depthTexture
)
	: device(device), depthTexture(depthTexture) {
	CreateScreenQuad();
	CreateShaders();
}

void RendererCompositor::BindShaderResources() {
	// Copy all the original values into our previous buffer.
	device->GetStreamSource(
		0,
		&previous.streamSource,
		&previous.streamOffset,
		&previous.streamStride
	);
	device->GetFVF(&previous.fvf);
	device->GetVertexShader(&previous.vertexShader);
	device->GetPixelShader(&previous.pixelShader);
	device->GetSamplerState(
		0, D3DSAMP_ADDRESSU, reinterpret_cast<DWORD *>(&previous.addressU1)
	);
	device->GetSamplerState(
		0, D3DSAMP_ADDRESSV, reinterpret_cast<DWORD *>(&previous.addressV1)
	);
	device->GetSamplerState(
		0, D3DSAMP_MAGFILTER, reinterpret_cast<DWORD *>(&previous.magFilter1)
	);
	device->GetSamplerState(
		0, D3DSAMP_MINFILTER, reinterpret_cast<DWORD *>(&previous.minFilter1)
	);
	device->GetSamplerState(
		0, D3DSAMP_MIPFILTER, reinterpret_cast<DWORD *>(&previous.mipFilter1)
	);
	device->GetTexture(0, &previous.texture0);
	device->GetRenderState(D3DRS_LIGHTING, &previous.lighting);

	// Bind vertex buffer
	DX("Failed to set stream source",
	   device->SetStreamSource(0, screenQuad.Get(), 0, sizeof(NDCVertex)));

	DX("Failed to set FVF", device->SetFVF(NDCVertex::FVF));

	// Bind shaders
	DX("Failed to set vertex shader (composite)",
	   device->SetVertexShader(vertexShader.Get()));
	DX("Failed to set pixel shader (composite)",
	   device->SetPixelShader(pixelShader.Get()));

	// Bind depth texture
	DX("Failed to set texture", device->SetTexture(0, depthTexture));

	// Bind sampler
	// Our sampler must sample pixel perfect.
	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	DX("Failed to set render state",
	   device->SetRenderState(D3DRS_LIGHTING, FALSE));
}

void RendererCompositor::RestorePreviousState() {
	if (previous.streamSource) {
		DX("Failed to restore stream source",
		   device->SetStreamSource(
			   0,
			   previous.streamSource,
			   previous.streamOffset,
			   previous.streamStride
		   ));
	}

	if (previous.vertexShader) {
		DX("Failed to restore vertex shader",
		   device->SetVertexShader(previous.vertexShader));
	}

	if (previous.pixelShader) {
		DX("Failed to restore pixel shader",
		   device->SetPixelShader(previous.pixelShader));
	}

	if (previous.texture0) {
		DX("Failed to restore texture",
		   device->SetTexture(0, previous.texture0));
	}

	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_ADDRESSU, previous.addressU1));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_ADDRESSV, previous.addressV1));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_MAGFILTER, previous.magFilter1));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_MINFILTER, previous.minFilter1));
	DX("Failed to restore sampler state",
	   device->SetSamplerState(0, D3DSAMP_MIPFILTER, previous.mipFilter1));
	DX("Failed to set render state",
	   device->SetRenderState(D3DRS_LIGHTING, previous.lighting));
}

void RendererCompositor::Composite() {
	BindShaderResources();
	DX("Failed to draw!", device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2));
	// Restore, giving back control to GMod.
	// This is essential because of technical limitations that force us to
	// arbitrarily set up our own rendering pipeline.
	RestorePreviousState();
}
