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
    float2 pixelSize = 1.f / res;

    int filterSize = 4;
    int samplesTaken = 0;

    for (int x = -filterSize; x <= filterSize; x++) {
        for (int y = -filterSize; y <= filterSize; y++) {
            float2 offset = float2(x, y) * pixelScale;
            float depthTap = depth.Sample(depthSampler, input.Texcoord + offset).r;
            if (depthTap == 1.0f) {
                continue;
            }

            depthTap = min(depthTap, 1.0f);
                sum += depthTap;
                samplesTaken++;
        }
    }

    sum /= samplesTaken;

    output.SmoothedDepth = float4(sum, 0, 0, 1);
    return output;
}