cbuffer cbPerFrame : register(b0) {
	float4x4 matView;
	float4x4 matProj;
}

float4 main(float4 pos) : SV_Position {
	return pos;
}