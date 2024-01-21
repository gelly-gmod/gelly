#ifndef MARCHING_CUBES_LUT_H
#define MARCHING_CUBES_LUT_H

#include <DirectXMath.h>

using namespace DirectX;

/**
 * \brief This namespace contains several lookup tables used by the marching
 * cubes algorithm.
 */
namespace gcr::marching_cubes::lut {
constexpr XMINT3 NEIGHBORS[27] = {
	XMINT3{-1, -1, -1}, XMINT3{0, -1, -1}, XMINT3{1, -1, -1}, XMINT3{-1, 0, -1},
	XMINT3{0, 0, -1},	XMINT3{1, 0, -1},  XMINT3{-1, 1, -1}, XMINT3{0, 1, -1},
	XMINT3{1, 1, -1},	XMINT3{-1, -1, 0}, XMINT3{0, -1, 0},  XMINT3{1, -1, 0},
	XMINT3{-1, 0, 0},	XMINT3{0, 0, 0},   XMINT3{1, 0, 0},	  XMINT3{-1, 1, 0},
	XMINT3{0, 1, 0},	XMINT3{1, 1, 0},   XMINT3{-1, -1, 1}, XMINT3{0, -1, 1},
	XMINT3{1, -1, 1},	XMINT3{-1, 0, 1},  XMINT3{0, 0, 1},	  XMINT3{1, 0, 1},
	XMINT3{-1, 1, 1},	XMINT3{0, 1, 1},   XMINT3{1, 1, 1},
};

}

#endif	// MARCHING_CUBES_LUT_H
