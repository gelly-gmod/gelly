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

constexpr uint32_t HashAlignedPosition(const XMINT3 &position) {
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

class DensityField {
	// we're using unbounded hash tables--so this wont ever map to physical
	// dimensions of the field, but rather to the number of buckets in the hash
	// table
	static constexpr uint32_t m_fieldCapacity = 1048576;

private:
	structs::HashTable<XMINT3, float, decltype(HashAlignedPosition)>
		m_densityField;

	structs::HashTable<XMINT3, uint16_t, decltype(HashAlignedPosition)>
		m_countField;

	float m_voxelSize;
	float m_isoValue;
	float m_supportRadius;

	/**
	 * \brief Loads and returns a density value from the density field, does not
	 * perform any sampling. \param x \param y \param z \return The density
	 * value at the given position.
	 */
	float Load(int32_t x, int32_t y, int32_t z) const;
	XMINT3 AlignToGrid(const XMFLOAT3 &position) const;

public:
	inline DensityField(float voxelSize = 1.0f, float isoValue = 100.0f);
	~DensityField() = default;

	float Sample(const XMFLOAT3 &position) const;
	bool IsInside(const XMFLOAT3 &position) const;

	void ComputeField(
		XMFLOAT4 *points,
		uint32_t pointCount,
		const XMINT3 min,
		const XMINT3 max
	);

	/**
	 * \brief Visualizes the density field given the maximum and minimum
	 * \param visualDebugFacility Debug facility to use for visualization.
	 */
	void Visualize(
		debugging::IVisualDebugFacility *visualDebugFacility,
		const XMINT3 min,
		const XMINT3 max
	) const;
};
}  // namespace detail

struct Output {
	vector<XMFLOAT3> m_vertices;
	vector<uint32_t> m_indices;
	vector<XMFLOAT3> m_normals;
};

struct Input {
	XMFLOAT4 *m_points;
	uint32_t m_pointCount;
};

Output March(const Input &input);

}  // namespace gcr::marching_cubes

#define MARCHING_CUBES_IMPLEMENTATION
#ifdef MARCHING_CUBES_IMPLEMENTATION
#include <gelly-cpu-refs/algo/marching-cubes-lut.h>

using namespace gcr::marching_cubes;
using namespace gcr::marching_cubes::detail;

#pragma region DensityField
inline XMINT3 DensityField::AlignToGrid(const XMFLOAT3 &position) const {
	const XMFLOAT3 middlePosition = XMFLOAT3{
		position.x + m_voxelSize / 2.0f,
		position.y + m_voxelSize / 2.0f,
		position.z + m_voxelSize / 2.0f,
	};

	const auto &alignedPosition = XMFLOAT3{
		floorf(middlePosition.x / m_voxelSize),
		floorf(middlePosition.y / m_voxelSize),
		floorf(middlePosition.z / m_voxelSize),
	};

	return XMINT3{
		static_cast<int32_t>(alignedPosition.x),
		static_cast<int32_t>(alignedPosition.y),
		static_cast<int32_t>(alignedPosition.z),
	};
}

inline float DensityField::Load(int32_t x, int32_t y, int32_t z) const {
	const XMINT3 position{x, y, z};
	const auto *density = m_densityField.Find(position);

	if (density == nullptr) {
		return 0.0f;
	}

	return *density;
}

inline float DensityField::Sample(const XMFLOAT3 &position) const {
	const auto alignedPosition = AlignToGrid(position);
	const auto density =
		Load(alignedPosition.x, alignedPosition.y, alignedPosition.z);

	return density;
}

inline bool DensityField::IsInside(const XMFLOAT3 &position) const {
	return Sample(position) >= m_isoValue;
}

inline void DensityField::ComputeField(
	XMFLOAT4 *points, uint32_t pointCount, const XMINT3 min, const XMINT3 max
) {
	m_countField.Clear();
	m_densityField.Clear();

	// need to populate the count field
	for (uint32_t i = 0; i < pointCount; i++) {
		const XMFLOAT4 point = points[i];
		const auto alignedPosition =
			AlignToGrid(XMFLOAT3{point.x, point.y, point.z});

		m_countField.Increment(alignedPosition, 1);
	}

	for (int32_t x = min.x; x < max.x; x++) {
		for (int32_t y = min.y; y < max.y; y++) {
			for (int32_t z = min.z; z < max.z; z++) {
				const XMINT3 position = AlignToGrid(XMFLOAT3{
					static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(z)
				});

				float maxDensity = 0.f;
				for (const auto &neighborOffset : lut::NEIGHBORS) {
					maxDensity += static_cast<float>(*m_countField.Find(XMINT3{
						position.x + neighborOffset.x,
						position.y + neighborOffset.y,
						position.z + neighborOffset.z,
					}));
					// Our weight, which is W(0, r) evaluates to 1 so for
					// simplicity it's omitted here
				}
				m_densityField.Insert(position, maxDensity);
			}
		}
	}
}

inline void DensityField::Visualize(
	debugging::IVisualDebugFacility *visualDebugFacility,
	const XMINT3 min,
	const XMINT3 max
) const {
	for (int32_t x = min.x; x < max.x; x++) {
		for (int32_t y = min.y; y < max.y; y++) {
			for (int32_t z = min.z; z < max.z; z++) {
				const XMINT3 position = AlignToGrid(XMFLOAT3{
					static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(z)
				});

				const auto *density = m_densityField.Find(position);

				if (density == nullptr) {
					continue;
				}

				const auto densityValue = std::min(*density, 255.f);

				const float interfacePosition[3] = {
					static_cast<float>(position.x),
					static_cast<float>(position.y),
					static_cast<float>(position.z),
				};

				const float voxelSize[3] = {
					m_voxelSize,
					m_voxelSize,
					m_voxelSize,
				};

				visualDebugFacility->Draw3DCube(
					&interfacePosition[0],
					&voxelSize[0],
					densityValue,
					densityValue,
					densityValue
				);
			}
		}
	}
}

DensityField::DensityField(float voxelSize, float isoValue)
	: m_densityField(m_fieldCapacity),
	  m_countField(m_fieldCapacity),
	  m_voxelSize(voxelSize),
	  m_isoValue(isoValue),
	  m_supportRadius(voxelSize * 2.f) {}
#pragma endregion

#endif

#endif	// MARCHING_CUBES_H