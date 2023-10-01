#include "CTexture.h"

#include "hooking/VTable.h"

static const uintptr_t GetTextureHandleIndex = 42;

ShaderAPITextureHandle_t GetCTextureHandle(CTexture *texture) {
	// Spawn a really cheap VTable to call the function we need.
	VTable textureVTable(*texture, 43);

	return textureVTable.CallOriginal<ShaderAPITextureHandle_t>(
		GetTextureHandleIndex, texture, 0, 0  // 0th frame
	);
}

const char *GetCTextureName(CTexture *texture) {
	VTable textureVTable(*texture, 43);

	return textureVTable.CallOriginal<const char *>(0, texture);
}