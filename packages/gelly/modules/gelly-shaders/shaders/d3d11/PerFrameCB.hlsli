cbuffer cbPerFrame : register(b0) {
	float2 res;
	float padding;
	float fov;
	float4x4 matProj;
	float4x4 matView;
#ifdef SHADERED
	float4x4 matGeo;
	float4x4 matInvGeo;
	float3 dir;
#endif
	float4x4 matInvProj;
	float4x4 matInvView;
	float3 eye;
	float particleRadius;
};