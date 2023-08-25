struct PS_INPUT {
	float4 Position : SV_Position;
	float2 Texcoord : TEXCOORD;
	float Depth : DEPTH;
};

float4 main(PS_INPUT input) : SV_TARGET {
	float2 normal = input.Texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
	float mag = dot(normal, normal);
	float3 fullNormal = float3(normal, sqrt(1.0 - mag));
	
	if (mag > 1.0f) {
		discard;
	}
	
	float4 nudgedPosition = input.Position - float4(fullNormal, 0.0) * 0.1f;
	return float4(nudgedPosition.z / nudgedPosition.w);
}