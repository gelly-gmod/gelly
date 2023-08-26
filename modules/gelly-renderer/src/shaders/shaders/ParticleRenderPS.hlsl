cbuffer cbPerFrame : register(b0) {
	float4x4 matProj;
}

struct PS_INPUT {
	float4 Position : SV_Position;
	float4 Center : CENTER;
	float2 Texcoord : TEXCOORD;
	float Depth : DEPTH;
};

float LinearizeDepth(float depth, float near, float far) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

struct PS_OUTPUT {
	float4 DepthCol : SV_TARGET;
	float Depth : SV_Depth;
};

PS_OUTPUT main(PS_INPUT input) {
	float2 normal = input.Texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
	float mag = dot(normal, normal);
	float3 fullNormal = float3(normal, sqrt(1.0 - mag));
	
	if (mag > 1.0f) {
		discard;
	}
	
	float4 nudgedPosition = input.Center + float4(fullNormal, 0) * 0.1f;
	nudgedPosition = mul(nudgedPosition, matProj);
	float4 depth = nudgedPosition.z;
	PS_OUTPUT output;
	output.DepthCol = depth;
	output.Depth = nudgedPosition.z / nudgedPosition.w;
	
	return output;
}