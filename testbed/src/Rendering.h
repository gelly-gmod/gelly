#ifndef GELLY_RENDERING_H
#define GELLY_RENDERING_H

#include <DirectXMath.h>
#include <d3d11.h>

#include <vector>

#include "ILogger.h"

using namespace DirectX;

namespace testbed {
using float3 = XMFLOAT3;
using float4 = XMFLOAT4;

constexpr float3 UP_VECTOR = float3(0.0f, 1.0f, 0.0f);

using MeshDataVector = std::vector<uint8_t>;
struct WorldMesh {
	MeshDataVector vertices;
	MeshDataVector normals;
	MeshDataVector indices;
};

using MeshReference = unsigned int;

// TODO: This does not map well at all to the scene graph
struct WorldRenderObject {
	MeshReference mesh;
	XMFLOAT4X4 transform;
};

using WorldRenderList = std::vector<WorldRenderObject>;

struct Camera {
	float3 position;
	float3 dir;

	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
};

struct WorldRenderCBuffer {
	XMFLOAT4X4 mvp;
	XMFLOAT4X4 invMvp;
	XMFLOAT4 eyePos;
	XMFLOAT4 windowSize;
};

ID3D11Device *InitializeRenderer(ILogger *newLogger);
void StartFrame();
void EndFrame();
MeshReference CreateWorldMesh(const WorldMesh &mesh);
void DestroyWorldMesh(const MeshReference &mesh);
void RenderWorldList(const WorldRenderList &list, const Camera &camera);
// TODO: When there is a stronger base, we can add fullscreen passes
}  // namespace testbed

#endif	// GELLY_RENDERING_H
