#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include <DirectXMath.h>
#include <gelly-cpu-refs/Logging.h>
#include <gelly-cpu-refs/debugging/IVisualDebugFacility.h>
#include <gelly-cpu-refs/structs/HashTable.h>

#include <cstdint>
#include <optional>
#include <vector>

using namespace DirectX;
using std::vector;

namespace gcr::marching_cubes {
namespace detail {
inline uint32_t HashAlignedPosition(const XMINT3 &position) {
	// var h = (xi * 92837111) ^ (yi * 689287499) ^ (zi * 283923481);
	// src:
	// https://github.com/matthias-research/pages/blob/3f3f48c5ae1fe1a1e43786db34fe2eea4ef6ae42/tenMinutePhysics/11-hashing.html#L67C6-L67C68

	return std::abs(
		(position.x * 92837111) ^ (position.y * 689287499) ^
		(position.z * 283923481)
	);
}

struct GridCell {
	/**
	 * \brief 8-bit unsigned integer representing the state of the cell.
	 */
	uint8_t m_index;
};
}  // namespace detail

class NeighborAcceleration {
	constexpr static uint32_t STARTING_CAPACITY = 1048576;
	constexpr static uint32_t MAX_PARTICLES_IN_CELL = 32;

private:
	struct Cell {
		uint32_t m_particleCount;
		uint32_t m_particleIndices[MAX_PARTICLES_IN_CELL];
	};

	structs::HashTable<XMINT3, Cell> m_cells;
	// usually the cell size is the same as the smoothing radius, which is h =
	// 2r.
	// This means that to perform a neighborhood search, we just need to look
	// into the 27 cells around the particle.
	float m_cellSize;
	float m_supportRadius;

	XMINT3 GetCellPosition(const XMFLOAT3 &position) const;

public:
	NeighborAcceleration(float particleRadius);

}

struct Output {
	vector<XMFLOAT3> m_vertices;
	vector<uint32_t> m_indices;
	vector<XMFLOAT3> m_normals;
};

struct Input {
	XMFLOAT4 *m_points;
	uint32_t m_pointCount;
	XMINT3 m_min;
	XMINT3 m_max;
	NeighborAcceleration *m_densityField;
	debugging::IVisualDebugFacility *m_visualDebugFacility;
};

Output March(const Input &input);

}  // namespace gcr::marching_cubes

#ifdef MARCHING_CUBES_IMPLEMENTATION
#include <gelly-cpu-refs/algo/marching-cubes-lut.h>

using namespace gcr::marching_cubes;
using namespace gcr::marching_cubes::detail;

#pragma region Neighbor acceleration

#pragma endregion

#pragma region MarchingCubes
inline Output gcr::marching_cubes::March(const Input &input) {
	vector<XMFLOAT3> vertices;
	vector<uint32_t> indices;
	vector<XMFLOAT3> normals;

	// temp
	return Output{vertices, indices, normals};
}

#pragma endregion

#endif

#endif	// MARCHING_CUBES_H