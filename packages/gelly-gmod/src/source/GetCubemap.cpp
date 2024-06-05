#include "GetCubemap.h"

#include <stdexcept>

#include "../logging/global-macros.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "GellyDataTypes.h"
#include "Interface.h"
#include "MinHook.h"
#include "hooking/Library.h"

using CShaderAPIDX8 = void *;
using CMaterialSystem = void *;
using CTexture = void *;

using namespace Gelly::DataTypes;

using TextureHandle_t = ulonglong;

using GetLocalCubemap_t = CTexture *(*__thiscall)(CMaterialSystem *);
using GetD3DTexture_t =
	IDirect3DBaseTexture9 *(*__thiscall)(CShaderAPIDX8 *, TextureHandle_t);
using GetTextureHandle_t =
	TextureHandle_t (*__thiscall)(CTexture *, void *, uint);
using GetLight_t = uintptr_t (*__thiscall)(CShaderAPIDX8 *, int);
using GetMaxLights_t = int (*__thiscall)(CShaderAPIDX8 *);
using AllowThreading_t = bool (*__thiscall)(CMaterialSystem *, bool, int);
using SetAmbientLightCube_t =
	void (*__thiscall)(CShaderAPIDX8 *, AmbientLightCube &);

static Library g_shaderAPI;
static Library g_materialSystem;

static GetLocalCubemap_t g_getLocalCubemap = nullptr;
static GetD3DTexture_t g_getD3DTexture = nullptr;
static GetTextureHandle_t g_getTextureHandle = nullptr;
static AllowThreading_t g_allowThreading = nullptr;
static GetLight_t g_getLight = nullptr;
static GetMaxLights_t g_getMaxLights = nullptr;

static SetAmbientLightCube_t g_setAmbientLightCube = nullptr;
static SetAmbientLightCube_t g_setAmbientLightCubeHk = nullptr;

static CMaterialSystem *g_matSys = nullptr;
static CShaderAPIDX8 *g_shaderAPIDX9 = nullptr;

static AmbientLightCube g_ambientLightCube;

#define LOG_IF_NULL(variable)               \
	if (!variable) {                        \
		LOG_WARNING(#variable " is null!"); \
	}

// We don't have to worry about half-sized registers, like EDX--as of writing
// the compiler has fit a reference to the cube into RDX
void __thiscall SetAmbientLightCubeHook(
	CShaderAPIDX8 *shaderAPI, AmbientLightCube &cube
) {
	std::memcpy(g_ambientLightCube, cube, sizeof(AmbientLightCube));
	g_setAmbientLightCubeHk(shaderAPI, cube);
}

void SetupAmbientLightCubeHook() {
	if (!g_setAmbientLightCube) {
		return;
	}

	if (MH_CreateHook(
			g_setAmbientLightCube,
			SetAmbientLightCubeHook,
			reinterpret_cast<LPVOID *>(&g_setAmbientLightCubeHk)
		) != MH_OK) {
		throw std::runtime_error("Failed to hook SetAmbientLightCube!");
	}

	MH_EnableHook(g_setAmbientLightCube);
}

void RemoveAmbientLightCubeHooks() {
	if (g_setAmbientLightCubeHk) {
		MH_RemoveHook(g_setAmbientLightCube);
		g_setAmbientLightCubeHk = nullptr;
	}
}

void EnsureAllHandlesInitialized() {
	if (!g_shaderAPI.IsInitialized()) {
		g_shaderAPI.Init("shaderapidx9.dll");
	}

	if (!g_materialSystem.IsInitialized()) {
		g_materialSystem.Init("materialsystem.dll");
	}

	if (g_getLocalCubemap && g_getD3DTexture && g_getTextureHandle &&
		g_allowThreading) {
		return;
	}
	LOG_INFO("Entering critical dbghelp section");
	g_getLocalCubemap = g_materialSystem.FindFunction<GetLocalCubemap_t>(
		sigs::CMaterialSystem_GetLocalCubemap
	);

	g_getD3DTexture = g_shaderAPI.FindFunction<GetD3DTexture_t>(
		sigs::CShaderAPIDX8_GetD3DTexture
	);

	g_getTextureHandle = g_materialSystem.FindFunction<GetTextureHandle_t>(
		sigs::CTexture_GetTextureHandle
	);

	g_allowThreading = g_materialSystem.FindFunction<AllowThreading_t>(
		sigs::CMaterialSystem_AllowThreading
	);

	g_getLight =
		g_shaderAPI.FindFunction<GetLight_t>(sigs::CShaderAPIDX8_GetLight);

	g_getMaxLights = g_shaderAPI.FindFunction<GetMaxLights_t>(
		sigs::CShaderAPIDX8_GetMaxLights
	);

	g_setAmbientLightCube = g_shaderAPI.FindFunction<SetAmbientLightCube_t>(
		sigs::CShaderAPIDX8_SetAmbientLightCube
	);
	LOG_INFO("Exiting critical dbghelp section");

	if (!g_getLocalCubemap || !g_getD3DTexture || !g_getTextureHandle ||
		!g_getLight || !g_getMaxLights || !g_setAmbientLightCube) {
		LOG_IF_NULL(g_getLocalCubemap);
		LOG_IF_NULL(g_getD3DTexture);
		LOG_IF_NULL(g_getTextureHandle);
		LOG_IF_NULL(g_getLight);
		LOG_IF_NULL(g_getMaxLights);
		LOG_IF_NULL(g_setAmbientLightCube);
		throw std::runtime_error("Failed to resolve all GetCubemap functions!");
	}

	if (!g_allowThreading) {
		throw std::runtime_error("Failed to resolve AllowThreading!");
	}

	g_matSys = GetInterface<CMaterialSystem>(
		"materialsystem.dll", "VMaterialSystem080"
	);

	g_shaderAPIDX9 =
		GetInterface<CShaderAPIDX8>("shaderapidx9.dll", "ShaderApi030");

	SetupAmbientLightCubeHook();
}

CTexture *GetLocalCubeMap() {
	EnsureAllHandlesInitialized();

	return g_getLocalCubemap(g_matSys);
}

IDirect3DBaseTexture9 *GetD3DTexture(TextureHandle_t texture) {
	EnsureAllHandlesInitialized();

	return g_getD3DTexture(g_shaderAPIDX9, texture);
}

TextureHandle_t GetTextureHandle(CTexture *texture, uint unknown2) {
	EnsureAllHandlesInitialized();

	return g_getTextureHandle(texture, nullptr, unknown2);
}

IDirect3DBaseTexture9 *GetCubemap() {
	CTexture *localCubemap = GetLocalCubeMap();

	if (!localCubemap) {
		return nullptr;
	}

	TextureHandle_t textureHandle = GetTextureHandle(localCubemap, 0);

	if (!textureHandle) {
		return nullptr;
	}

	IDirect3DBaseTexture9 *d3dTexture = GetD3DTexture(textureHandle);

	if (!d3dTexture) {
		return nullptr;
	}

	return d3dTexture;
}

void DisableMaterialSystemThreading() {
	EnsureAllHandlesInitialized();
	g_allowThreading(g_matSys, false, -1);
}

std::optional<LightDesc_t> GetLightDesc(int index) {
	EnsureAllHandlesInitialized();
	auto light =
		*reinterpret_cast<LightDesc_t *>(g_getLight(g_shaderAPIDX9, index));

	if (light.m_Type == MATERIAL_LIGHT_DISABLE) {
		return std::nullopt;
	}

	return light;
}

int GetMaxLights() {
	EnsureAllHandlesInitialized();
	return g_getMaxLights(g_shaderAPIDX9) -
		   1;  // I have no idea why it's -1, but it's reflected in the original
			   // decompiled code
}

AmbientLightCube *GetAmbientLightCube() {
	EnsureAllHandlesInitialized();
	// we can't be confident that the ambient light cube has been set yet
	return &g_ambientLightCube;
}