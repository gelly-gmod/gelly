#include "CShaderAPIDX8.h"

#include <cassert>
#include <cstdio>

#include "Interface.h"
#include "hooking/Library.h"
#include "hooking/VTable.h"

static CShaderAPIDX8 shaderAPIDX8 = nullptr;
static VTable shaderAPIVTable;

static const uintptr_t SetAmbientLightCubeIndex = 130;
static const uintptr_t GetD3DTextureIndex = 264;

static Library shaderAPILib;

static const char *GetD3DTextureSignature =
	"48 85 d2 75 03 33 c0 c3 80 7a 1a 01 48 8b";

typedef IDirect3DBaseTexture9 *(__thiscall *GetD3DTextureFn)(
	CShaderAPIDX8, ShaderAPITextureHandle_t
);

CShaderAPIDX8 GetShaderAPIDX8() {
	if (!shaderAPILib.IsInitialized()) {
		shaderAPILib.Init("shaderapidx9.dll");
	}

	if (shaderAPIDX8 == nullptr) {
		shaderAPIDX8 = reinterpret_cast<CShaderAPIDX8>(
			GetInterface<CShaderAPIDX8>("shaderapidx9.dll", SHADER_API_VERSION)
		);

#ifdef _DEBUG
		assert(shaderAPIDX8 != nullptr);
#endif

		shaderAPIVTable.Init(*shaderAPIDX8, 266);
	}

	return shaderAPIDX8;
}

#pragma optimize("", off)
IDirect3DBaseTexture9 *GetD3DTexture(CTexture *texture) {
	auto handle = GetCTextureHandle(texture);
	auto getD3DTexture =
		shaderAPILib.FindFunction<GetD3DTextureFn>(GetD3DTextureSignature);

	auto d3dTexture = getD3DTexture(shaderAPIDX8, handle);
	printf("d3dTexture: %p\n", d3dTexture);
	printf("thisPtr: %p\n", shaderAPIDX8);
	printf("test: %p\n", getD3DTexture);

	return d3dTexture;
}
