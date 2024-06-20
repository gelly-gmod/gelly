#ifndef CHECK_BIND_FLAGS_H
#define CHECK_BIND_FLAGS_H

#include <d3d11.h>

#include <optional>

namespace gelly::renderer::util {

struct ParsedBindFlags {
	bool isSRVRequired;
	bool isRTVRequired;
	bool isUAVRequired;
};

enum class PrimaryBindType { RTV, SRV, UAV, CONSTANT_BUFFER, VERTEX_BUFFER };

inline auto ParseBindFlags(const UINT &bindFlags) -> ParsedBindFlags {
	return {
		.isSRVRequired = (bindFlags & D3D11_BIND_SHADER_RESOURCE) != 0,
		.isRTVRequired = (bindFlags & D3D11_BIND_RENDER_TARGET) != 0,
		.isUAVRequired = (bindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0
	};
}

inline auto FindPrimaryBindType(const UINT &bindFlags)
	-> std::optional<PrimaryBindType> {
	// this is a non-primary bind type if it has more than one bit set
	if (__popcnt(bindFlags) != 1) {
		return std::nullopt;
	}

	if (D3D11_BIND_RENDER_TARGET & bindFlags) {
		return PrimaryBindType::RTV;
	}

	if (D3D11_BIND_SHADER_RESOURCE & bindFlags) {
		return PrimaryBindType::SRV;
	}

	if (D3D11_BIND_UNORDERED_ACCESS & bindFlags) {
		return PrimaryBindType::UAV;
	}

	if (D3D11_BIND_CONSTANT_BUFFER & bindFlags) {
		return PrimaryBindType::CONSTANT_BUFFER;
	}

	if (D3D11_BIND_VERTEX_BUFFER & bindFlags) {
		return PrimaryBindType::VERTEX_BUFFER;
	}
}
}  // namespace gelly::renderer::util

#endif	// CHECK_BIND_FLAGS_H
