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

static const char *IsTextureSignature =
	"48 85 d2 74 2e 48 39 52 70 75 06 48 39 52";

typedef IDirect3DBaseTexture9 *(__thiscall *GetD3DTextureFn)(
	CShaderAPIDX8, unsigned long long
);

typedef unsigned long long(__thiscall *IsTextureFn)(
	CShaderAPIDX8, void *, ShaderAPITextureHandle_t
);

// static HookedFunction IsTextureHook;
// static IsTextureFn IsTextureHookFnOrig;
//
// unsigned long long __thiscall IsTextureHookFn(
//	CShaderAPIDX8 thisPtr, void *edx, ShaderAPITextureHandle_t handle
//) {
//	printf("IsTextureHookFn\n");
//	printf("thisPtr: %p\n", thisPtr);
//	printf("handle: %p\n", handle);
//
//	return IsTextureHookFnOrig(thisPtr, edx, handle);
// }

CShaderAPIDX8 GetShaderAPIDX8() {
	if (!shaderAPILib.IsInitialized()) {
		shaderAPILib.Init("shaderapidx9.dll");

		//		shaderAPILib.HookFunction(
		//			IsTextureSignature,
		//			reinterpret_cast<void *>(IsTextureHookFn),
		//			reinterpret_cast<void **>(&IsTextureHookFnOrig),
		//			IsTextureHook
		//		);
		//
		//		IsTextureHook.Enable();
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

	//	auto isTexture =
	// shaderAPILib.FindFunction<IsTextureFn>(IsTextureSignature);
	//
	//	unsigned long long test = isTexture(shaderAPIDX8, handle);
	//
	//	printf("thisPtr: %p\n", shaderAPIDX8);
	//	printf("test: %p\n", test);

	IDirect3DBaseTexture9 *d3dTexture =
		getD3DTexture(GetShaderAPIDX8(), (unsigned long long)handle);

	printf("d3dTexture: %p\n", d3dTexture);

	return d3dTexture;
}
