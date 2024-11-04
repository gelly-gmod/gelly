#include "ThicknessStructs.hlsli"
#include "FluidRenderCBuffer.hlsli"

PS_OUTPUT main(GS_OUTPUT input) {
	float2 sphereUV = input.Tex * 2.f - 1.f;
	float sphereRadius = length(sphereUV);

	if (sphereRadius > 1.f) {
		discard;
	}

	PS_OUTPUT output = (PS_OUTPUT)0;
	output.Thickness = 0.01f;
	return output;
}