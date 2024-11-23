#ifndef FLEX_TYPES_H
#define FLEX_TYPES_H

namespace FleX {
struct Float4 {
	float x, y, z, w;
};

struct Float3 {
	float x, y, z;
};

using Quat = Float4;
}  // namespace FleX
#endif	// FLEX_TYPES_H
