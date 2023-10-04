#include "CShaderAPIDX8.h"

#include <cassert>

#include "Interface.h"
#include "hooking/Library.h"
#include "hooking/VTable.h"

static CShaderAPIDX8 shaderAPIDX8 = nullptr;
static Library shaderAPILib;

static const char *GetD3DTextureSignature =
	"48 85 d2 75 03 33 c0 c3 80 7a 1a 01 48 8b";

typedef IDirect3DBaseTexture9 *(__thiscall *GetD3DTextureFn)(
	CShaderAPIDX8, unsigned long long
);

static GetD3DTextureFn getD3DTexture = nullptr;

void EnsureShaderAPIDX8() {
	if (!shaderAPILib.IsInitialized()) {
		shaderAPILib.Init("shaderapidx9.dll");
		getD3DTexture =
			shaderAPILib.FindFunction<GetD3DTextureFn>(GetD3DTextureSignature);
	}

	if (shaderAPIDX8 == nullptr) {
		shaderAPIDX8 = reinterpret_cast<CShaderAPIDX8>(
			GetInterface<CShaderAPIDX8>("shaderapidx9.dll", SHADER_API_VERSION)
		);

#ifdef _DEBUG
		assert(shaderAPIDX8 != nullptr);
		assert(getD3DTexture != nullptr);
#endif
	}
}

#pragma optimize("", off)
IDirect3DBaseTexture9 *GetD3DTexture(CTexture *texture) {
	if (texture == nullptr) {
		return nullptr;
	}

	EnsureShaderAPIDX8();

	auto handle = GetCTextureHandle(texture);

	IDirect3DBaseTexture9 *d3dTexture =
		getD3DTexture(shaderAPIDX8, (unsigned long long)handle);

	return d3dTexture;
}
