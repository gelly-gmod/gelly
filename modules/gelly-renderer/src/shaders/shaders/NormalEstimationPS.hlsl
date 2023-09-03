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

float3 ReconstructViewPosition(float2 texcoord)
{
	float z = depth.Sample(DepthSampler, texcoord).r;
	if (z == 0.0f)
	{
		return float3(0.0f, 0.0f, 0.0f);
	}

	// We need to go back from viewport transform to view space.
	// That's basically: viewport -> ndc -> clip -> view
	float2 projTexCoord = (texcoord * 2.0f) - 1.0f;
	float4 position = float4(projTexCoord, z, 1.0f);
	position = mul(position, matInvProj);
	position /= position.w;

	return position.xyz;
}

float4 EstimateNormal(float2 i) {
	float4 normal = depth.Sample(DepthSampler, i);
	if (normal.a == 0.f) {
		return float4(0.f, 0.f, 0.f, 0.f);
	}

	normal.x *= -1.0f;
	normal.y *= -1.0f;

	float3 viewDir = normalize(float4(0.4, 0.6, 0.3, 1.0f));
	float lightLevel = dot(normal.xyz, viewDir) * 0.4f;

	return float4(lightLevel, lightLevel, lightLevel, 1.f);
}

float4 main(VS_OUTPUT input) : SV_TARGET {
	return EstimateNormal(input.Texcoord);
}