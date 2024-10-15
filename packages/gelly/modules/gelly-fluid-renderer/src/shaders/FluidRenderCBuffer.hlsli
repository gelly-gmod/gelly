// C++ and HLSL interop macros, we use these to define the constant buffer in
// HLSL and C++ to avoid code duplication.

#ifdef __cplusplus
#define CBUFFER_DECLARATION(name) struct name
#else
#define CBUFFER_DECLARATION(name) cbuffer name : register(b0)
#endif

#ifdef __cplusplus
namespace gelly::renderer::cbuffer {
struct float4x4 {
	float m[4][4];
};

struct float2 {
	float x;
	float y;
};

struct float3 {
	float x;
	float y;
	float z;
};

using uint = unsigned int;
#endif
CBUFFER_DECLARATION(FluidRenderCBufferData) {
	float4x4 g_View;
	float4x4 g_Projection;
	float4x4 g_InverseView;
	float4x4 g_InverseProjection;

	float g_ViewportWidth;
	float g_ViewportHeight;
	float g_ThresholdRatio;
	float g_ParticleRadius;

	float g_NearPlane;
	float g_FarPlane;
	float2 padding;

	float3 g_CameraPosition;
	float g_DiffuseScale;

	float g_DiffuseMotionBlur;
	float3 padding2;

	uint g_SmoothingPassIndex;
	float3 padding3;

	float2 g_InvViewport;
	float2 padding4;
};
#ifdef __cplusplus
}
#endif