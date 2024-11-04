#include "ThicknessStructs.hlsli"

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = float4(input.Pos.xyz, 1.f);
	return output;
}