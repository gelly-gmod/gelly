cbuffer cbPerFrame : register(b0) {
	float2 res;
	float4x4 matProj;
};

Texture2D depth : register(t0);

SamplerState DepthSampler {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_OUTPUT {
	float4 Position : SV_Position;
	float2 Texcoord : TEXCOORD;
};

float3 EstimateNormal(float2 i) {
	float depthLeft = depth.Sample(DepthSampler, i - float2(1.0 / res.x, 0)).x;
	float depthRight = depth.Sample(DepthSampler, i + float2(1.0 / res.x, 0)).x;
	float depthUp = depth.Sample(DepthSampler, i - float2(0, 1.0 / res.y)).x;
	float depthDown = depth.Sample(DepthSampler, i + float2(0, 1.0 / res.y)).x;
	
	// Calculate finite differences in the x and y directions
	float ddxDepth = depthRight - depthLeft;
	float ddyDepth = depthDown - depthUp;
	
	// Construct the normal vector
	float3 gradient = normalize(float3(-ddxDepth, -ddyDepth, 0.0f));
	
	// Invert the gradient to get the normal vector (assuming positive depth is towards the camera).
	float3 normal = gradient;
	
	// Optionally, normalize the normal vector to ensure it's a unit vector.
	normal = normalize(normal);
	
	return normal;
}

float4 main(VS_OUTPUT input) : SV_TARGET {
	return float4(EstimateNormal(input.Texcoord) * 0.5 + 0.5, 1.0f);
}