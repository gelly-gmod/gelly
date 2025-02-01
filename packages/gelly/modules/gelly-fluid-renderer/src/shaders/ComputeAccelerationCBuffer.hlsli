// C++ and HLSL interop macros, we use these to define the constant buffer in
// HLSL and C++ to avoid code duplication.

#ifdef __cplusplus
#define CBUFFER_DECLARATION(name) struct name
#else
#define CBUFFER_DECLARATION(name) cbuffer name : register(b0)
#endif

#ifdef __cplusplus
namespace gelly::renderer::cbuffer {
#ifndef CBUFFER_CPP_GLUE_CODE
#define CBUFFER_CPP_GLUE_CODE
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
#endif
CBUFFER_DECLARATION(ComputeAccelerationCBufferData) {
	float g_DeltaTime;
	float g_DTPadding[3];
};
#ifdef __cplusplus
}
#endif