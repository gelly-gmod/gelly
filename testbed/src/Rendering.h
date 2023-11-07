#ifndef GELLY_RENDERING_H
#define GELLY_RENDERING_H

#include <DirectXMath.h>

using namespace DirectX;

namespace testbed {
struct float3 {
	float x;
	float y;
	float z;
};

using MeshReference = unsigned int;

struct Mesh {
	float3 *vertices;
	unsigned int vertexCount;
	float3 *normals;
	unsigned int normalCount;
};

// TODO: This does not map well at all to the scene graph
struct RenderObject {
	MeshReference mesh;
	XMFLOAT4X4 transform;
};

struct RenderList {
	RenderObject *objects;
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

void RenderWorldList(const RenderList &list, const Camera &camera);
// TODO: When there is a stronger base, we can add fullscreen passes
}  // namespace testbed

#endif	// GELLY_RENDERING_H
