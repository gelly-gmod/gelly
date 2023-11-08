#ifndef GELLY_RENDERING_H
#define GELLY_RENDERING_H

#include <DirectXMath.h>

using namespace DirectX;

namespace testbed {
using float3 = XMFLOAT3;
using float4 = XMFLOAT4;

constexpr float3 UP_VECTOR = float3(0.0f, 1.0f, 0.0f);

struct WorldMesh {
	float3 *vertices;
	unsigned int vertexCount;
	float3 *normals;
	unsigned int normalCount;

	unsigned short *indices;
	unsigned int indexCount;
};

using MeshReference = unsigned int;

// TODO: This does not map well at all to the scene graph
struct WorldRenderObject {
	MeshReference mesh;
	XMFLOAT4X4 transform;
};

struct WorldRenderList {
	WorldRenderObject *objects;
	unsigned int objectCount;
};

struct Camera {
	float3 position;
	float3 dir;

	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
};

struct WorldRenderCBuffer {
	XMFLOAT4X4 model;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

void InitializeRenderer();
void StartFrame();
void EndFrame();
MeshReference CreateWorldMesh(const WorldMesh &mesh);
void DestroyWorldMesh(MeshReference mesh);
void RenderWorldList(const WorldRenderList &list, const Camera &camera);

// TODO: When there is a stronger base, we can add fullscreen passes
}  // namespace testbed

#endif	// GELLY_RENDERING_H
