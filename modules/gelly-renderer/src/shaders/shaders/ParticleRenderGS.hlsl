cbuffer cbPerFrame : register(b0) {
	float4x4 matView;
	float4x4 matProj;

	float4x4 matGeo;

}

struct GS_OUTPUT {
	float4 Position : SV_Position;
	float2 Texcoord : TEXCOORD;
	float Depth : DEPTH;
};

struct VS_INPUT {
	float4 Position : SV_Position;
};

[maxvertexcount(4)]
void main(point VS_INPUT input[1], inout TriangleStream<GS_OUTPUT> stream) {
	float scale = 0.1f;
	
	float4 origin = mul(input[0].Position, matView);

	origin = mul(mul(input[0].Position, matGeo), matView);

    float2 topLeft = float2(-scale, -scale);
    float2 topRight = float2(scale, -scale);
    float2 bottomLeft = float2(-scale, scale);
    float2 bottomRight = float2(scale, scale);
    
	GS_OUTPUT output;
	output.Position = mul(origin + float4(topLeft, 0.0f, 0.0f), matProj);
	output.Texcoord = float2(0, 0);
	stream.Append(output);
	output.Position = mul(origin + float4(topRight, 0.0f, 0.0f), matProj);
	output.Texcoord = float2(1, 0);
	stream.Append(output);
	output.Position = mul(origin + float4(bottomLeft, 0.0f, 0.0f), matProj);
	output.Texcoord = float2(0, 1);
	stream.Append(output);
	output.Position = mul(origin + float4(bottomRight, 0.0f, 0.0f), matProj);
	output.Texcoord = float2(1, 1);
	stream.Append(output);
}