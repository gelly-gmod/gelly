#ifndef GELLY_RENDERING_H
#define GELLY_RENDERING_H

#include <DirectXMath.h>
#include <d3d11.h>

#include <array>
#include <vector>

#include "ILogger.h"

using namespace DirectX;

#define GBUFFER_ALBEDO_TEXNAME "gbuffer/albedo"
#define GBUFFER_NORMAL_TEXNAME "gbuffer/normal"
#define GBUFFER_DEPTH_TEXNAME "gbuffer/depth"

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

struct GenericRenderCBuffer {
	XMFLOAT4X4 mvp;
	XMFLOAT4X4 invMvp;
	XMFLOAT4 eyePos;
	XMFLOAT4 windowSize;
};

ID3D11Device *InitializeRenderer(ILogger *newLogger);
/**
 * \brief This function takes in the result of InitializeRenderer and returns
 * the device context for the renderer if the device is correct and valid.
 * \param device Device of the renderer
 * \return Device context of the renderer
 */
ID3D11DeviceContext *GetRendererContext(ID3D11Device *device);

ID3D11Texture2D *GetBackBuffer(ID3D11Device *device);
ID3D11RenderTargetView *GetBackBufferRTV(ID3D11Device *device);

void StartFrame();
void EndFrame();
MeshReference CreateWorldMesh(const WorldMesh &mesh);
void DestroyWorldMesh(const MeshReference &mesh);
void RenderWorldList(const WorldRenderList &list, const Camera &camera);

}  // namespace testbed

#endif	// GELLY_RENDERING_H
