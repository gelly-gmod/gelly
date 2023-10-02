#include "CTexture.h"

#include "hooking/Library.h"
#include "hooking/VTable.h"

static const uintptr_t GetTextureHandleIndex = 42;
static Library materialsystemLib;

static const char *GetTextureHandleSignature =
	"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 33 ff 8b da";

typedef ShaderAPITextureHandle_t(__thiscall *GetTextureHandleFn)(void *, int);

ShaderAPITextureHandle_t GetCTextureHandle(CTexture *texture) {
	if (!materialsystemLib.IsInitialized()) {
		materialsystemLib.Init("materialsystem.dll");
	}

	// Spawn a really cheap VTable to call the function we need.
	//	VTable textureVTable(*texture, 43);
	//
	//	return textureVTable.CallOriginal<ShaderAPITextureHandle_t>(
	//		GetTextureHandleIndex, texture, 0, 0  // 0th frame
	//	);

	auto getTextureHandle = materialsystemLib.FindFunction<GetTextureHandleFn>(
		GetTextureHandleSignature
	);

	return getTextureHandle(texture, 0);
}
// TODO: FIX TEXTURE HANDLE, THE VTABLE IS NOT RIGHT FOR THIS USE SIG
const char *GetCTextureName(CTexture *texture) {
	VTable textureVTable(*texture, 43);

	return textureVTable.CallOriginal<const char *>(0, texture);
}