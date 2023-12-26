#include <GarrysMod/Lua/Interface.h>
#include "LoggingMacros.h"
#include <cstdio>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <GMFS.h>
#include <MinHook.h>
#include <Windows.h>

#include "Gelly.h"
#include "source/D3DDeviceWrapper.h"
#include "source/IBaseClientDLL.h"

#define DEFINE_LUA_FUNC(namespace, name) \
	LUA->PushCFunction(namespace##_##name); \
	LUA->SetField(-2, #name);

static GellyIntegration *gelly = nullptr;

void InjectConsoleWindow() {
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
}

LUA_FUNCTION(gelly_Render) {
	gelly->Render();
	return 0;
}

LUA_FUNCTION(gelly_Simulate) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Number); // Delta time
	auto dt = static_cast<float>(LUA->GetNumber(1));

	gelly->Simulate(dt);
	return 0;
}

LUA_FUNCTION(gelly_LoadMap) {
	LUA->CheckType(1, GarrysMod::Lua::Type::String); // Map name
	gelly->LoadMap(LUA->GetString(1));
	return 0;
}

LUA_FUNCTION(gelly_EmitCube) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Vector); // Position
	LUA->CheckType(2, GarrysMod::Lua::Type::Vector); // Size
	LUA->CheckType(3, GarrysMod::Lua::Type::Number); // Density

	Vector position = LUA->GetVector(1);
	Vector size = LUA->GetVector(2);

	ISimCommandList *commandList = gelly->GetSimulation()->CreateCommandList();
	for (int i = 0; i < static_cast<int>(LUA->GetNumber(3)); i++) {
		float x = rand() / static_cast<float>(RAND_MAX);
		float y = rand() / static_cast<float>(RAND_MAX);
		float z = rand() / static_cast<float>(RAND_MAX);

		// Center at position
		x -= 0.5f;
		y -= 0.5f;
		z -= 0.5f;

		x *= size.x;
		y *= size.y;
		z *= size.z;

		AddParticle particle = {x + position.x, y + position.y, z + position.z};
		commandList->AddCommand({ADD_PARTICLE, particle});
	}
	gelly->GetSimulation()->ExecuteCommandList(commandList);
	gelly->GetSimulation()->DestroyCommandList(commandList);

	return 0;
}

LUA_FUNCTION(gelly_GetComputeDeviceName) {
	LUA->PushString(gelly->GetComputeDeviceName());
	return 1;
}

LUA_FUNCTION(gelly_GetActiveParticles) {
	LUA->PushNumber(gelly->GetSimulation()->GetSimulationData()->GetActiveParticles());
	return 1;
}

GMOD_MODULE_OPEN() {
	InjectConsoleWindow();
	if (const auto status = FileSystem::LoadFileSystem(); status != FILESYSTEM_STATUS::OK) {
		LOG_ERROR("Failed to load file system: %d", status);
		return 0;
	}

	if (const auto status = MH_Initialize(); status != MH_OK) {
		LOG_ERROR("Failed to initialize MinHook: %s", MH_StatusToString(status));
		return 0;
	}

	LOG_INFO("Hello, world!");
	LOG_INFO("Grabbing initial information...");

	CViewSetup currentView = {};
	GetClientViewSetup(currentView);

	LOG_INFO("Screen resolution: %dx%d", currentView.width, currentView.height);
	LOG_INFO("Field of view: %f", currentView.fov);
	LOG_INFO("Near clip: %f", currentView.zNear);
	LOG_INFO("Far clip: %f", currentView.zFar);

	auto* d3dDevice = GetD3DDevice();
	LOG_INFO("D3D9Ex device: %p", d3dDevice);

	D3DCAPS9 caps = {};
	d3dDevice->GetDeviceCaps(&caps);
	LOG_INFO("D3D9Ex device caps:");
	LOG_INFO("  Max texture width: %lu", caps.MaxTextureWidth);
	LOG_INFO("  Max texture height: %lu", caps.MaxTextureHeight);
	LOG_INFO("  Max texture aspect ratio: %lu", caps.MaxTextureAspectRatio);
	LOG_INFO("  Max texture repeat: %lu", caps.MaxTextureRepeat);
	LOG_INFO("  Texture filter caps: %lu", caps.TextureFilterCaps);

	gelly = new GellyIntegration(currentView.width, currentView.height, d3dDevice);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->CreateTable();
	DEFINE_LUA_FUNC(gelly, Render);
	DEFINE_LUA_FUNC(gelly, Simulate);
	DEFINE_LUA_FUNC(gelly, GetComputeDeviceName);
	DEFINE_LUA_FUNC(gelly, GetActiveParticles);
	DEFINE_LUA_FUNC(gelly, EmitCube);
	DEFINE_LUA_FUNC(gelly, LoadMap);
	LUA->SetField(-2, "gelly");
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	LOG_INFO("Goodbye, world!");
	if (const auto status = MH_Uninitialize(); status != MH_OK) {
		LOG_ERROR("Failed to uninitialize MinHook: %s", MH_StatusToString(status));
		return 0;
	}
	delete gelly;
	return 0;
}
