#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"
#include "source-engine/AmbientCube.hlsli"

sampler2D foamTex : register(s0);
sampler2D depthTex : register(s1);

float4 eyePos : register(c0);
float4 refractAndCubemapStrength : register(c1);

struct CompositeLight {
    float4 LightInfo;
    float4 Position;
    float4 Enabled;
};

CompositeLight lights[2] : register(c2); // next reg that can be used is c8 (2 + 6)
float4 aspectRatio : register(c8);
float4 ambientCube[6] : register(c9);
float4x4 viewProjMatrix : register(c17);
float4 sunDir : register(c21);
float4 lightScaling : register(c22);
float4x4 invViewProjMatrix : register(c23);

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
    output.Color = float4(irradiance, min(foam.b, 1.f)); // We do want it to blend with the background
    output.Depth = depth;
    return output;
}
