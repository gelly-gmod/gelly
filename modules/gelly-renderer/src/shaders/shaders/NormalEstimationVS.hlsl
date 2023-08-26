struct VS_INPUT {
	float4 Position : SV_Position;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 Position : SV_Position;
	float2 Texcoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	// No transformation required since the geometry is just covering the entire
	// screen in NDC space.
	output.Position = input.Position;
	output.Texcoord = input.Texcoord;
	
	return output;
}