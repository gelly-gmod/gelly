#include "GetCubemap.h"

#include <stdexcept>

#include "GellyDataTypes.h"
#include "Interface.h"
#include "hooking/Library.h"

using CShaderAPIDX8 = void*;
using CMaterialSystem = void*;
using CTexture = void*;

using namespace Gelly::DataTypes;

using TextureHandle_t = ulonglong;

using GetLocalCubemap_t = CTexture* (*)(CMaterialSystem*);
using GetD3DTexture_t = IDirect3DBaseTexture9* (*)(CShaderAPIDX8*, CTexture*);
using GetTextureHandle_t = TextureHandle_t(*)(CTexture*, void*, uint);

static Library g_shaderAPI;
static Library g_materialSystem;

static GetLocalCubemap_t g_getLocalCubemap = nullptr;
static GetD3DTexture_t g_getD3DTexture = nullptr;
static GetTextureHandle_t g_getTextureHandle = nullptr;

static CMaterialSystem* g_matSys = nullptr;
static CShaderAPIDX8* g_shaderAPIDX9 = nullptr;

void EnsureAllHandlesInitialized() {
	if (!g_shaderAPI.IsInitialized()) {
		g_shaderAPI.Init("shaderapidx9.dll");
	}

	if (!g_materialSystem.IsInitialized()) {
		g_materialSystem.Init("materialsystem.dll");
	}

	if (g_getLocalCubemap && g_getD3DTexture && g_getTextureHandle) {
		return;
	}

	g_getLocalCubemap = g_materialSystem.FindFunction<GetLocalCubemap_t>(sigs::CMaterialSystem_GetLocalCubemap);
	g_getD3DTexture = g_shaderAPI.FindFunction<GetD3DTexture_t>(sigs::CShaderAPIDX8_GetD3DTexture);
	g_getTextureHandle = g_materialSystem.FindFunction<GetTextureHandle_t>(sigs::CTexture_GetTextureHandle);

	if (!g_getLocalCubemap || !g_getD3DTexture || !g_getTextureHandle) {
		throw std::runtime_error("Failed to resolve all GetCubemap functions!");
	}

	g_matSys = GetInterface<CMaterialSystem>("materialsystem.dll", "VMaterialSystem080");
	g_shaderAPIDX9 = GetInterface<CShaderAPIDX8>("shaderapidx9.dll", "ShaderApi030");
}

CTexture* GetLocalCubeMap() {
	EnsureAllHandlesInitialized();

	return g_getLocalCubemap(g_matSys);
}

IDirect3DBaseTexture9* GetD3DTexture(CTexture* texture) {
	EnsureAllHandlesInitialized();

	return g_getD3DTexture(g_shaderAPIDX9, texture);
}

TextureHandle_t GetTextureHandle(CTexture* texture, ulonglong unknown2) {
	EnsureAllHandlesInitialized();

	return g_getTextureHandle(texture, nullptr, unknown2);
}

IDirect3DBaseTexture9* GetCubemap() {
	CTexture* localCubemap = GetLocalCubeMap();

	if (!localCubemap) {
		return nullptr;
	}

	IDirect3DBaseTexture9* d3dTexture = GetD3DTexture(localCubemap);

	if (!d3dTexture) {
		return nullptr;
	}

	TextureHandle_t textureHandle = GetTextureHandle(localCubemap, 0);

	if (!textureHandle) {
		return nullptr;
	}

	return d3dTexture;
}
