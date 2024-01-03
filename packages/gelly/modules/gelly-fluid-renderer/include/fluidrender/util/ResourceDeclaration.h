#ifndef RESOURCEDECLARATION_H
#define RESOURCEDECLARATION_H

#include <variant>

#include "../IRenderContext.h"

namespace Gelly::util {
struct Tex2D {
	struct Proportion {
		float width;
		float height;
	};

	struct Absolute {
		uint width;
		uint height;
	};

	using Dimensions = std::variant<Proportion, Absolute>;
	std::string name;
	Dimensions dimensions;
	TextureDesc desc;
};

constexpr Tex2D::Dimensions FULLSCREEN =
	Tex2D::Dimensions{Tex2D::Proportion{1.f, 1.f}};
constexpr Tex2D::Dimensions QUARTERSCREEN =
	Tex2D::Dimensions{Tex2D::Proportion{0.25f, 0.25f}};

using Buffer = BufferDesc;
using BufferLayout = BufferLayoutDesc;
using DepthBuffer = DepthBufferDesc;
using DeclaredResource = std::variant<Tex2D, Buffer, BufferLayout, DepthBuffer>;
using ResourceDeclaration = std::vector<DeclaredResource>;
}  // namespace Gelly::util

#endif	// RESOURCEDECLARATION_H
