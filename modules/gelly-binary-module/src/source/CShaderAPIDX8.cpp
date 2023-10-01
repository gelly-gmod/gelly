#include "CShaderAPIDX8.h"

#include <cassert>

#include "Interface.h"
#include "hooking/VTable.h"

static CShaderAPIDX8 shaderAPIDX8 = nullptr;
static VTable shaderAPIVTable;

static const uintptr_t SetAmbientLightCubeIndex = 130;
static const uintptr_t GetD3DTextureIndex = 264;

CShaderAPIDX8 GetShaderAPIDX8() {
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

IDirect3DBaseTexture9 *GetD3DTexture(CTexture *texture) {
	auto handle = GetCTextureHandle(texture);
	return shaderAPIVTable.CallOriginal<IDirect3DBaseTexture9 *>(
		GetD3DTextureIndex, shaderAPIDX8, handle
	);
}
