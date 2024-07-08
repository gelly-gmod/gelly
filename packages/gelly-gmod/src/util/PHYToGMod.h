#ifndef PHYTOGMOD_H
#define PHYTOGMOD_H

namespace gelly::gmod::helpers {
constexpr float PHY_SCALE_FACTOR = 39.3701f;  // meters to inches
inline auto ConvertPHYPositionToGMod(float &x, float &y, float &z) -> void {
	x *= PHY_SCALE_FACTOR;
	const auto tempY = y;
	y = z * PHY_SCALE_FACTOR;
	z = -tempY * PHY_SCALE_FACTOR;
}
}  // namespace gelly::gmod::helpers
#endif	// PHYTOGMOD_H
