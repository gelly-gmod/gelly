#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include <DirectXMath.h>
#include <gelly-cpu-refs/debugging/IVisualDebugFacility.h>

#include <cstdint>
#include <vector>

#include "gelly-cpu-refs/structs/HashTable.h"

using namespace DirectX;
using std::vector;

namespace gcr::marching_cubes {
namespace detail {
static constexpr uint16_t MAX_PARTICLES_PER_CELL = 8;
static constexpr float INTERPOLATION_EPSILON = 0.00001f;
static constexpr float CENTRAL_DIFFERENCING_DELTA = 1.f;

inline uint32_t HashAlignedPosition(const XMUINT3 &position) {
	// var h = (xi * 92837111) ^ (yi * 689287499) ^ (zi * 283923481);
	// src:
	// https://github.com/matthias-research/pages/blob/3f3f48c5ae1fe1a1e43786db34fe2eea4ef6ae42/tenMinutePhysics/11-hashing.html#L67C6-L67C68

	return (position.x * 92837111) ^ (position.y * 689287499) ^
		   (position.z * 283923481);
}

inline float M4SplineKernel(float r, float h) {
	// m4 spline kernel is piecewise defined

	const float inversePiH3 = 1.0f / (XM_PI * h * h * h);

	if (r < 0.0f) {
		r = -r;
	}

	// piecewise definition
	float piecewiseTerm = 0.f;
	const float rOverH = r / h;

	if (r <= h) {
		piecewiseTerm = 1 - (3 / 2) * (rOverH * rOverH) +
						(3 / 4) * (rOverH * rOverH * rOverH);
	} else if (r <= 2 * h) {
		const float twoMinusRHRatio = (2 - rOverH);

		piecewiseTerm =
			(1 / 4) * (twoMinusRHRatio * twoMinusRHRatio * twoMinusRHRatio);
	}

	// if r > 2h then piecewiseTerm is 0 but implicit in the definition

	return inversePiH3 * piecewiseTerm;
}

struct GridCell {
	/**
	 * \brief 8-bit unsigned integer representing the state of the cell.
	 */
	int m_index;

	// Densities for each corner of the cell, basically the density of the
	// verts before binning them into the cell which is what m_index is for
	float m_vertDensities[8];

	XMFLOAT3 m_vertPositions[8];
};

inline XMFLOAT3 InterpolateVertex(
	const XMFLOAT3 &startVertex,
	const XMFLOAT3 &endVertex,
	const float startVertexDensity,
	const float endVertexDensity,
	float isoValue
) {
	if (fabsf(isoValue - startVertexDensity) < INTERPOLATION_EPSILON) {
		return startVertex;
	}

	if (fabsf(isoValue - endVertexDensity) < INTERPOLATION_EPSILON) {
		return endVertex;
	}

	if (fabsf(startVertexDensity - endVertexDensity) < INTERPOLATION_EPSILON) {
		return startVertex;
	}

	float interpolationRatio = (isoValue - startVertexDensity) /
							   (endVertexDensity - startVertexDensity);

	XMFLOAT3 interpolatedPoint = {};
	interpolatedPoint.x =
		startVertex.x + interpolationRatio * (endVertex.x - startVertex.x);
	interpolatedPoint.y =
		startVertex.y + interpolationRatio * (endVertex.y - startVertex.y);
	interpolatedPoint.z =
		startVertex.z + interpolationRatio * (endVertex.z - startVertex.z);

	return interpolatedPoint;
}
}  // namespace detail

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
	debugging::IVisualDebugFacility *m_visualDebugFacility;
};

struct Settings {
	float m_radius;
	float m_isovalue;
	/**
	 * \brief Describes the side length of a single voxel.
	 * This parameter and the domain size are used to discretize the space into
	 * voxels that are m_voxelSize units in length.
	 *
	 * \note If the domain is 100 units in length and m_voxelSize is
	 * 10, then there will be 10 voxels in each dimension, or 1000 voxels total.
	 */
	float m_voxelSize;
};

Output March(const Input &input, const Settings &settings);

}  // namespace gcr::marching_cubes

#ifdef MARCHING_CUBES_IMPLEMENTATION
#include <gelly-cpu-refs/algo/marching-cubes-lut.h>

using namespace gcr::marching_cubes;
using namespace gcr::marching_cubes::detail;

inline Output gcr::marching_cubes::March(
	const Input &input, const Settings &settings
) {
	vector<XMFLOAT3> vertices;
	vector<uint32_t> indices;
	vector<XMFLOAT3> normals;

	vertices.reserve(1000000);
	indices.reserve(1000000);
	normals.reserve(1000000);

	XMUINT3 domain = XMUINT3{
		static_cast<uint32_t>(input.m_max.x - input.m_min.x),
		static_cast<uint32_t>(input.m_max.y - input.m_min.y),
		static_cast<uint32_t>(input.m_max.z - input.m_min.z)
	};

	// 2h^2, primarily for distance calculations
	const float smoothingRadius2Squared =
		settings.m_radius * 4.0f * settings.m_radius * 4.0f;

	// h, primarily for density calculations
	const float smoothingRadius = settings.m_radius * 2.0f;

	uint32_t voxelsPerUnit = settings.m_voxelSize;
	if (settings.m_voxelSize < 1.0f) {
		// sizes less than 1.0f mean that we need to discretize the voxels
		// themselves, so it turns into 1 voxel / m_voxelSize units
		// technically speaking the only valid values for m_voxelSize less than
		// 1 is 0.25 since there's not exactly a correct way to discretize a
		// voxel into 0.5 units
		voxelsPerUnit = 1.f / settings.m_voxelSize;
	}

	const uint32_t totalCellCount = domain.x * domain.y * domain.z *
									voxelsPerUnit * voxelsPerUnit *
									voxelsPerUnit;

	// dense grid representation of the discretized space
	// in most usecases a space-skipping algorithm should be used to avoid
	// meshing empty space

	// 0-indexed buffer which contains a list of all particles in a cell
	// access would look like: cellIndex * MAX_PARTICLES_PER_CELL +
	// particleIndex (0-MAX_PARTICLES_PER_CELL)
	uint32_t *particleIndexBuffer =
		new uint32_t[totalCellCount * MAX_PARTICLES_PER_CELL];

	uint32_t *particleCountBuffer = new uint32_t[totalCellCount];
	memset(particleCountBuffer, 0, sizeof(uint32_t) * totalCellCount);

	GridCell *cells = new GridCell[totalCellCount];

	// first pass: assign particles to cells
	// iterate over all particles and assign them to their respective cells
	// second pass: iterate over all cells and compute their index + scalar
	// vertices by using smoothed density values
	// third pass: iterate over all cells and generate triangles
	// fourth pass: iterate over all triangles and generate vertex normals using
	// central differencing
	// fifth pass: make duplicate vertices unique and generate indices

	const auto alignPositionToGrid =
		[&domain, &voxelsPerUnit, &input, &settings](const XMFLOAT3 &position) {
			XMFLOAT3 localPosition = position;
			localPosition.x -= input.m_min.x;
			localPosition.y -= input.m_min.y;
			localPosition.z -= input.m_min.z;

			XMUINT3 gridPosition = XMUINT3{
				static_cast<uint32_t>(localPosition.x / settings.m_voxelSize),
				static_cast<uint32_t>(localPosition.y / settings.m_voxelSize),
				static_cast<uint32_t>(localPosition.z / settings.m_voxelSize)
			};

			gridPosition.x = std::max(gridPosition.x, 0u);
			gridPosition.y = std::max(gridPosition.y, 0u);
			gridPosition.z = std::max(gridPosition.z, 0u);

			gridPosition.x = std::min(gridPosition.x, domain.x - 1);
			gridPosition.y = std::min(gridPosition.y, domain.y - 1);
			gridPosition.z = std::min(gridPosition.z, domain.z - 1);

			return gridPosition;
		};

	const auto alignedPositionToIndex =
		[&domain, &voxelsPerUnit](const XMUINT3 &position) {
			const XMUINT3 scaledDomain = XMUINT3{
				domain.x * voxelsPerUnit,
				domain.y * voxelsPerUnit,
				domain.z * voxelsPerUnit
			};

			return position.x + position.y * scaledDomain.x +
				   position.z * scaledDomain.x * scaledDomain.y;
		};

	for (uint32_t i = 0; i < input.m_pointCount; i++) {
		const XMFLOAT4 &position = input.m_points[i];
		const XMUINT3 gridPosition =
			alignPositionToGrid(XMFLOAT3{position.x, position.y, position.z});

		const uint32_t cellIndex = alignedPositionToIndex(gridPosition);
		const uint32_t currentCount = particleCountBuffer[cellIndex];

		if (currentCount >= MAX_PARTICLES_PER_CELL - 1) {
			continue;
		}

		particleCountBuffer[cellIndex] = currentCount + 1;
		particleIndexBuffer[cellIndex * MAX_PARTICLES_PER_CELL + currentCount] =
			i;
	}

	const auto computeDensityAtPosition = [&alignPositionToGrid,
										   &alignedPositionToIndex,
										   &particleCountBuffer,
										   &particleIndexBuffer,
										   &smoothingRadius,
										   &smoothingRadius2Squared,
										   &input,
										   &domain,
										   &settings](const XMFLOAT3 &position
										  ) {
		float density = 0.0f;
		const XMUINT3 &gridPosition = alignPositionToGrid(position);
		XMINT3 signedGridPosition = XMINT3{
			static_cast<int32_t>(gridPosition.x),
			static_cast<int32_t>(gridPosition.y),
			static_cast<int32_t>(gridPosition.z)
		};

		XMFLOAT3 densityCube = XMFLOAT3{
			static_cast<float>(signedGridPosition.x),
			static_cast<float>(signedGridPosition.y),
			static_cast<float>(signedGridPosition.z)
		};

		float size[3] = {
			settings.m_voxelSize, settings.m_voxelSize, settings.m_voxelSize
		};

		for (const auto &neighborOffset : lut::NEIGHBORS) {
			XMINT3 neighborPosition;
			neighborPosition.x = signedGridPosition.x + neighborOffset.x;
			neighborPosition.y = signedGridPosition.y + neighborOffset.y;
			neighborPosition.z = signedGridPosition.z + neighborOffset.z;

			XMUINT3 neighborGridPosition = XMUINT3{
				static_cast<uint32_t>(neighborPosition.x),
				static_cast<uint32_t>(neighborPosition.y),
				static_cast<uint32_t>(neighborPosition.z)
			};

			neighborGridPosition.x = std::max(neighborGridPosition.x, 0u);
			neighborGridPosition.y = std::max(neighborGridPosition.y, 0u);
			neighborGridPosition.z = std::max(neighborGridPosition.z, 0u);

			neighborGridPosition.x =
				std::min(neighborGridPosition.x, domain.x - 1);
			neighborGridPosition.y =
				std::min(neighborGridPosition.y, domain.y - 1);
			neighborGridPosition.z =
				std::min(neighborGridPosition.z, domain.z - 1);

			const uint32_t neighborCellIndex =
				alignedPositionToIndex(neighborGridPosition);

			// since density is a summation when simplified we can just add
			// by each neighbor's kernel contribution and not worry about
			// the particle count

			const uint32_t neighborParticleCount =
				particleCountBuffer[neighborCellIndex];

			for (uint32_t i = 0; i < neighborParticleCount; i++) {
				const uint32_t particleIndex = particleIndexBuffer
					[neighborCellIndex * MAX_PARTICLES_PER_CELL + i];

				const XMFLOAT4 &particlePosition =
					input.m_points[particleIndex];

				const XMVECTOR particlePositionVector =
					XMLoadFloat4(&particlePosition);
				const XMVECTOR positionVector = XMLoadFloat3(&position);

				const XMVECTOR distanceVector =
					XMVectorSubtract(particlePositionVector, positionVector);

				const float distanceSquared =
					fabsf(XMVectorGetX(XMVector3LengthSq(distanceVector)));

				if (distanceSquared > smoothingRadius2Squared) {
					// No point running the kernel if the distance is greater
					// than 2h which literally evaluates to 0
					continue;
				}

				density +=
					M4SplineKernel(sqrtf(distanceSquared), smoothingRadius);

				// ensure its normalized and not greater than 1
				density = std::min(density, 1.0f);
			}
		}

		return density;
	};

	const auto computeCentralDifferenceAtPoint =
		[&computeDensityAtPosition,
		 &alignedPositionToIndex,
		 &particleCountBuffer,
		 &particleIndexBuffer,
		 &smoothingRadius,
		 &smoothingRadius2Squared,
		 &input,
		 &domain,
		 &settings](const XMFLOAT3 &position) {
			XMFLOAT3 normal = {};
			XMFLOAT3 posXPositive = {
				position.x + CENTRAL_DIFFERENCING_DELTA, position.y, position.z
			};
			XMFLOAT3 posYPositive = {
				position.x, position.y + CENTRAL_DIFFERENCING_DELTA, position.z
			};
			XMFLOAT3 posZPositive = {
				position.x, position.y, position.z + CENTRAL_DIFFERENCING_DELTA
			};

			XMFLOAT3 posXNegative = {
				position.x - CENTRAL_DIFFERENCING_DELTA, position.y, position.z
			};
			XMFLOAT3 posYNegative = {
				position.x, position.y - CENTRAL_DIFFERENCING_DELTA, position.z
			};
			XMFLOAT3 posZNegative = {
				position.x, position.y, position.z - CENTRAL_DIFFERENCING_DELTA
			};

			normal.x =
				-(computeDensityAtPosition(posXPositive) -
				  computeDensityAtPosition(posXNegative));
			normal.y =
				-(computeDensityAtPosition(posYPositive) -
				  computeDensityAtPosition(posYNegative));
			normal.z =
				-(computeDensityAtPosition(posZPositive) -
				  computeDensityAtPosition(posZNegative));

			return normal;
		};

	// now we can calculate the index and scalar vertices for each cell in the
	// **scaled** domain, not the original domain

	// O(n^3 + 8) complexity, but we can parallelize this
	for (uint32_t x = 0; x < domain.x * voxelsPerUnit; x++) {
		for (uint32_t y = 0; y < domain.y * voxelsPerUnit; y++) {
			for (uint32_t z = 0; z < domain.z * voxelsPerUnit; z++) {
				XMFLOAT3 cellPos = {
					static_cast<float>(x) / voxelsPerUnit,
					static_cast<float>(y) / voxelsPerUnit,
					static_cast<float>(z) / voxelsPerUnit
				};

				XMFLOAT3 cellSize = {
					settings.m_voxelSize,
					settings.m_voxelSize,
					settings.m_voxelSize
				};

				uint32_t cellIndex = alignedPositionToIndex(XMUINT3{x, y, z});
				cells[cellIndex].m_index = 0;

				for (uint32_t i = 0; i < 8; i++) {
					XMFLOAT3 vertexPositionVector = XMFLOAT3{
						static_cast<float>(cellPos.x) +
							lut::CUBE_VERTEX_OFFSETS_CENTERED[i].x *
								(settings.m_voxelSize) +
							input.m_min.x,
						static_cast<float>(cellPos.y) +
							lut::CUBE_VERTEX_OFFSETS_CENTERED[i].y *
								(settings.m_voxelSize) +
							input.m_min.y,
						static_cast<float>(cellPos.z) +
							lut::CUBE_VERTEX_OFFSETS_CENTERED[i].z *
								(settings.m_voxelSize) +
							input.m_min.z
					};

					const float density =
						computeDensityAtPosition(vertexPositionVector);

					cells[cellIndex].m_vertDensities[i] = density;

					if (density > settings.m_isovalue) {
						cells[cellIndex].m_index |= (1 << i);
					}

					cells[cellIndex].m_vertPositions[i] = vertexPositionVector;
				}
			}
		}
	}

	uint32_t indexCounter = 0;

	for (uint32_t x = 0; x < domain.x * voxelsPerUnit; x++) {
		for (uint32_t y = 0; y < domain.y * voxelsPerUnit; y++) {
			for (uint32_t z = 0; z < domain.z * voxelsPerUnit; z++) {
				uint32_t cellIndex = alignedPositionToIndex(XMUINT3{x, y, z});

				XMFLOAT3 triangleVertices[12] = {};
				GridCell &cell = cells[cellIndex];

				if (cell.m_index == 0xFF) {
					continue;
				}

				if (lut::EDGE_TABLE[cell.m_index] == 0) {
					continue;
				}

				if (lut::EDGE_TABLE[cell.m_index] & 1) {
					triangleVertices[0] = InterpolateVertex(
						cell.m_vertPositions[0],
						cell.m_vertPositions[1],
						cell.m_vertDensities[0],
						cell.m_vertDensities[1],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 2) {
					triangleVertices[1] = InterpolateVertex(
						cell.m_vertPositions[1],
						cell.m_vertPositions[2],
						cell.m_vertDensities[1],
						cell.m_vertDensities[2],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 4) {
					triangleVertices[2] = InterpolateVertex(
						cell.m_vertPositions[2],
						cell.m_vertPositions[3],
						cell.m_vertDensities[2],
						cell.m_vertDensities[3],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 8) {
					triangleVertices[3] = InterpolateVertex(
						cell.m_vertPositions[3],
						cell.m_vertPositions[0],
						cell.m_vertDensities[3],
						cell.m_vertDensities[0],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 16) {
					triangleVertices[4] = InterpolateVertex(
						cell.m_vertPositions[4],
						cell.m_vertPositions[5],
						cell.m_vertDensities[4],
						cell.m_vertDensities[5],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 32) {
					triangleVertices[5] = InterpolateVertex(
						cell.m_vertPositions[5],
						cell.m_vertPositions[6],
						cell.m_vertDensities[5],
						cell.m_vertDensities[6],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 64) {
					triangleVertices[6] = InterpolateVertex(
						cell.m_vertPositions[6],
						cell.m_vertPositions[7],
						cell.m_vertDensities[6],
						cell.m_vertDensities[7],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 128) {
					triangleVertices[7] = InterpolateVertex(
						cell.m_vertPositions[7],
						cell.m_vertPositions[4],
						cell.m_vertDensities[7],
						cell.m_vertDensities[4],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 256) {
					triangleVertices[8] = InterpolateVertex(
						cell.m_vertPositions[0],
						cell.m_vertPositions[4],
						cell.m_vertDensities[0],
						cell.m_vertDensities[4],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 512) {
					triangleVertices[9] = InterpolateVertex(
						cell.m_vertPositions[1],
						cell.m_vertPositions[5],
						cell.m_vertDensities[1],
						cell.m_vertDensities[5],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 1024) {
					triangleVertices[10] = InterpolateVertex(
						cell.m_vertPositions[2],
						cell.m_vertPositions[6],
						cell.m_vertDensities[2],
						cell.m_vertDensities[6],
						settings.m_isovalue
					);
				}

				if (lut::EDGE_TABLE[cell.m_index] & 2048) {
					triangleVertices[11] = InterpolateVertex(
						cell.m_vertPositions[3],
						cell.m_vertPositions[7],
						cell.m_vertDensities[3],
						cell.m_vertDensities[7],
						settings.m_isovalue
					);
				}

				for (int i = 0; lut::TRIANGLE_TABLE[cell.m_index][i] != -1;
					 i += 3) {
					const auto triTable = lut::TRIANGLE_TABLE[cell.m_index];

					// input.m_visualDebugFacility->Draw3DLine(
					// 	&triangleVertices[triTable[i]].x,
					// 	&triangleVertices[triTable[i + 1]].x,
					// 	1,
					// 	0,
					// 	0
					// );
					//
					// input.m_visualDebugFacility->Draw3DLine(
					// 	&triangleVertices[triTable[i + 1]].x,
					// 	&triangleVertices[triTable[i + 2]].x,
					// 	0,
					// 	1,
					// 	0
					// );
					//
					// input.m_visualDebugFacility->Draw3DLine(
					// 	&triangleVertices[triTable[i + 2]].x,
					// 	&triangleVertices[triTable[i]].x,
					// 	0,
					// 	0,
					// 	1
					// );

					vertices.push_back(triangleVertices[triTable[i]]);
					vertices.push_back(triangleVertices[triTable[i + 1]]);
					vertices.push_back(triangleVertices[triTable[i + 2]]);

					indices.push_back(indexCounter++);

					normals.push_back(computeCentralDifferenceAtPoint(
						triangleVertices[triTable[i]]
					));
					normals.push_back(computeCentralDifferenceAtPoint(
						triangleVertices[triTable[i + 1]]
					));
					normals.push_back(computeCentralDifferenceAtPoint(
						triangleVertices[triTable[i + 2]]
					));
				}
			}
		}
	}

	delete[] particleIndexBuffer;
	delete[] particleCountBuffer;
	delete[] cells;

	return Output{vertices, indices, normals};
}

#endif

#endif	// MARCHING_CUBES_H