#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"
#include "source-engine/AmbientCube.hlsli"

sampler2D foamTex : register(s0);
sampler2D depthTex : register(s1);
float4 ambientCube[6] : register(c9);

struct PS_OUTPUT {
    float4 Color : SV_TARGET0;
    float Depth : SV_DEPTH;
};

PS_OUTPUT main(VS_INPUT input) {
    float4 foam = tex2D(foamTex, input.Tex);
    float depth = tex2D(depthTex, input.Tex).b;
    if (depth >= 1.f) {
        discard;
    }
    
    PS_OUTPUT output = (PS_OUTPUT)0;
	float3 irradiance = SampleAmbientCube(ambientCube, float3(0.f, 0.f, 1.f));
    irradiance = max(irradiance, SampleAmbientCube(ambientCube, float3(0.f, 0.f, -1.f)));
    irradiance = max(irradiance, SampleAmbientCube(ambientCube, float3(0.f, 1.f, 0.f)));
    irradiance = max(irradiance, SampleAmbientCube(ambientCube, float3(0.f, -1.f, 0.f)));
    irradiance = max(irradiance, SampleAmbientCube(ambientCube, float3(1.f, 0.f, 0.f)));
    irradiance = max(irradiance, SampleAmbientCube(ambientCube, float3(-1.f, 0.f, 0.f)));

    output.Color = float4(irradiance, min(foam.b, 1.f)); // We do want it to blend with the background
    output.Depth = depth;
    return output;
}
