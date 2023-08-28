cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
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

bool InvalidPartialDerivative(float x)
{
    return isnan(x) || isinf(x) || x > 0.2f;
}

float3 EstimateNormal(float2 i) {
	if (depth.Sample(DepthSampler, i).a == 0.0f) {
		return float3(0);
	}
	
	float depthLeft = depth.Sample(DepthSampler, i - float2(1.0 / res.x, 0)).x;
	float depthRight = depth.Sample(DepthSampler, i + float2(1.0 / res.x, 0)).x;
	float depthUp = depth.Sample(DepthSampler, i - float2(0, 1.0 / res.y)).x;
	float depthDown = depth.Sample(DepthSampler, i + float2(0, 1.0 / res.y)).x;
	
	// Calculate finite differences in the x and y directions
	float ddxDepth = depthRight - depthLeft;
	float ddyDepth = depthDown - depthUp;
	
	if (InvalidPartialDerivative(ddxDepth) || InvalidPartialDerivative(ddyDepth)) {
		discard;
	}
	
	// Construct the normal vector
	float3 gradient = float3(ddxDepth, ddyDepth, 0.001f);
	
	// Invert the gradient to get the normal vector (assuming positive depth is towards the camera).
	float3 normal = gradient;
	
	// Optionally, normalize the normal vector to ensure it's a unit vector.
	normal = normalize(normal);
	
	return normal;
}

float4 main(VS_OUTPUT input) : SV_TARGET {
	return float4(EstimateNormal(input.Texcoord), 1.0f);
	// return float4(depth.Sample(DepthSampler, input.Texcoord));
}