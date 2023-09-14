cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
#ifdef SHADERED
	float4x4 matGeo;
#endif
	float4x4 matInvProj;
	float4x4 matInvView;
	float3 eye;
	float particleRadius;
};