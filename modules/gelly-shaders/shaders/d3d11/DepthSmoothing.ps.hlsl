#include "NDCQuadStages.hlsli"
#include "PerFrameCB.hlsli"

struct PS_OUTPUT {
    float4 SmoothedDepth : SV_Target0;
};

static const float2 pixelScale = 1.f / res;

Texture2D depth : register(t0);
SamplerState depthSampler : register(s0);

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;

    // Simple gaussian blur
    float sum = 0.0f;
    float pixelSize = 1.f / res;

    int filterSize = 1;

    for (int x = -filterSize; x <= filterSize; x++) {
        for (int y = -filterSize; y <= filterSize; y++) {
            float2 offset = float2(x, y) * pixelScale;
            float depth = depth.Sample(depthSampler, input.Texcoord + offset).r;
            sum += depth;
        }
    }

    sum /= (filterSize * 2 + 1) * (filterSize * 2 + 1);

    output.SmoothedDepth = float4(sum, 0, 0, 0);
    return output;
}