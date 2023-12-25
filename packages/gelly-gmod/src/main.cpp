#include <GarrysMod/Lua/Interface.h>
#include "LoggingMacros.h"
#include <cstdio>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
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
	float dt = LUA->GetNumber(0);

	gelly->Simulate(dt);
	return 0;
}

LUA_FUNCTION(gelly_AddParticle) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Vector); // Position

	ISimCommandList *commandList = gelly->GetSimulation()->CreateCommandList();
	commandList->AddCommand({ADD_PARTICLE, AddParticle{
		LUA->GetVector(1).x,
		LUA->GetVector(1).y,
		LUA->GetVector(1).z
	}});
	gelly->GetSimulation()->ExecuteCommandList(commandList);
	gelly->GetSimulation()->DestroyCommandList(commandList);

	return 0;
}

GMOD_MODULE_OPEN() {
	InjectConsoleWindow();
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
	DEFINE_LUA_FUNC(gelly, AddParticle);
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
