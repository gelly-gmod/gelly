#ifndef CHECK_BIND_FLAGS_H
#define CHECK_BIND_FLAGS_H

#include <d3d11.h>

namespace gelly::renderer::util {

struct ParsedBindFlags {
	bool isSRVRequired;
	bool isRTVRequired;
	bool isUAVRequired;
};

inline auto ParseBindFlags(const UINT &bindFlags) -> ParsedBindFlags {
	return {
		.isSRVRequired = (bindFlags & D3D11_BIND_SHADER_RESOURCE) != 0,
		.isRTVRequired = (bindFlags & D3D11_BIND_RENDER_TARGET) != 0,
		.isUAVRequired = (bindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0
	};
}
}  // namespace gelly::renderer::util

#endif	// CHECK_BIND_FLAGS_H
