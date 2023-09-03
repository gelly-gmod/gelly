// Highly unstable and broken implementation of
// the narrow range filter
cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
	float4x4 matInvProj;
};

Texture2D depth : register(t0);

SamplerState DepthSampler {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct VS_OUTPUT {
	float4 Position : SV_Position;
	float2 Texcoord : TEXCOORD;
};


/**
* From equation 4
*/
float GaussianWeight(float2 i, float2 j, float stdDev) {
	float powerNumerator = -length(j - i);
	powerNumerator *= powerNumerator;
	
	float powerDenominator = 2 * (stdDev * stdDev);
	
	float power = powerNumerator / powerDenominator;
	
	return exp(power);
}

/**
* From equation 3
*/
float ComputeFilterWeight(float2 i, float2 j, float stdDev, float upperDepth) {
	float depth_i = depth.Sample(DepthSampler, i).a;
	float depth_j = depth.Sample(DepthSampler, j).a;
	
	if (depth_j > depth_i + upperDepth) {
		return 0.f;
	}
	
	float2 pixel_i = res * i;
	float2 pixel_j = res * j;
	
	return GaussianWeight(i, j, stdDev);
}

/**
* From equation 2
*/
float Clamping(float z_i, float z_j, float upper, float lower) {
	if (z_j >= z_i - upper) {
		return z_j;
	}
	
	return z_i - lower;
}

/**
* From equation 5
*/
float GetFilterSize(float worldSize, float depth) {
	float fovAngle = 2.0 * atan(1.0 / matProj[1][1]);
	float o_i = (res.y * worldSize) / (2.0 * depth * tan(fovAngle / 2.0));
	return 3 * o_i;
}

float3 FilterDepth(float2 i, float upper, float lower, float stdDev) {
	// i can be considered the origin of the filter
	float2 pixel_i = res * i;
	float3 numerator = float3(0, 0, 0);
	float3 denominator = 0;
	float depth_i = depth.Sample(DepthSampler, i).a;
	
	if (depth_i == 0) {
		return float3(0, 0, 0);
	}
	
	int filterSize = 5;
	for (int y = -filterSize; y <= filterSize; y++) {
		for (int x = -filterSize; x <= filterSize; x++) {
			float2 j = (pixel_i + float2(x, y)) / res;
			float weight = ComputeFilterWeight(i, j, stdDev, upper);
			float clamped = Clamping(depth.Sample(DepthSampler, i).x, depth.Sample(DepthSampler, j).x, upper, lower);
			float4 depth_j = depth.Sample(DepthSampler, j);

			numerator += depth_j.xyz * weight;
			denominator += weight;
		}
	}
	
	return numerator / denominator;
}

float4 main(VS_OUTPUT input) : SV_TARGET {
	float depth_i = depth.Sample(DepthSampler, input.Texcoord).a;
	
	if (depth_i == 0) {
		return float4(0, 0, 0, 0);
	}

	float3 newNormal = depth.Sample(DepthSampler, input.Texcoord).xyz;
	float3 testDir = mul(float4(0.3, 0.5, 0.9, 0.f), matView).xyz;
	testDir = normalize(testDir);
	float lighting = saturate(dot(newNormal, testDir)) * 0.7f;

	return float4(lighting, lighting, lighting, 1.f);
}