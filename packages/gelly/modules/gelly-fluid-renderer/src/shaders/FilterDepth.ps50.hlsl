#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"
#include "util/EyeToProjDepth.hlsli"
#include "util/IsUnderwater.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_Target0;
};

float sqr(float x) {
    return x * x;
}

float ComputeBlurScale() {
    float aspect = g_ViewportWidth / g_ViewportHeight;
    return g_ViewportWidth / aspect * g_Projection._m11;
}

float FetchEyeDepth(float2 pixel) {
    float eyeDepth = InputDepth.Load(int3(pixel, 0)).g;
	// only return negative if it is positive
	return sign(eyeDepth) == -1.f ? eyeDepth : -eyeDepth;
}

float FetchProjDepth(float2 pixel) {
    return InputDepth.Load(int3(pixel, 0)).r;
}

#define FILTER_RADIUS 2.f
float CreateIsosurfaceDepth(float2 tex) {
    float2 inPosition = tex * float2(g_ViewportWidth, g_ViewportHeight);
    const float blurRadiusWorld = g_ParticleRadius * 0.5f;
    const float blurScale = ComputeBlurScale();
    const float blurFalloff = g_ThresholdRatio;

    float depth = FetchEyeDepth(inPosition);
    float blurDepthFalloff = g_ThresholdRatio;
    float maxBlurRadius = FILTER_RADIUS;

    float radius = FILTER_RADIUS;
    float radiusInv = 1.0 / radius;
    float taps = ceil(radius);
    float frac = taps - radius;

    float sum = 0.0;
    float wsum = 0.0;
    float count = 0.0;

    for (float y = -FILTER_RADIUS; y <= FILTER_RADIUS; y += 1.0) {
        for (float x = -FILTER_RADIUS; x <= FILTER_RADIUS; x += 1.0) {
            float2 offset = float2(x, y);
            float sample = FetchEyeDepth(inPosition + offset);
            if (FetchProjDepth(inPosition + offset) >= 1.f) {
				// lower our offset to the closest valid pixel
                continue;
            }

            float r1 = length(float2(x, y)) * radiusInv;
            float w = exp(-(r1 * r1));

            float r2 = (sample - depth) * blurDepthFalloff;
            // 'g' is a Gaussian, but it can have some artifacts.
            // To remedy it, we use a wider Gaussian, which is
            // derived from the parametric Gaussian, then
            // simplified to exp(-(r^2/4))
            float g = exp(-(r2 * r2) / 8);
            float wBoundary = step(radius, max(abs(x), abs(y)));
            float wFrac = 1.0 - wBoundary * frac;

            sum += sample * w * g * wFrac;
            wsum += w * g * wFrac;
            count += g * wFrac;
        }
    }

    if (wsum > 0.0) {
        sum /= wsum;
	}

    float blend = count / sqr(2.0 * radius + 1.0);
    return lerp(depth, sum, blend);
}


PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float2 original = InputDepth.SampleLevel(InputDepthSampler, input.Tex, 0);
    if (original.r >= 1.f) {
        discard;
    }

	// there's really no point in filtering the depth if it's underwater
	if (IsProjDepthUnderwater(original)) {
		output.Color = float4(original, 0.f, 1.f);
		return output;
	}

    float eyeDepth = CreateIsosurfaceDepth(input.Tex);
	float projDepth = EyeToProjDepth(eyeDepth);
    output.Color = float4(projDepth, eyeDepth, 0.f, 1.0f);
    return output;
}