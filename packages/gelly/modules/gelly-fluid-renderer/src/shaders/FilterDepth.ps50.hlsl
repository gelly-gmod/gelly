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
    float eyeDepth = InputDepth.SampleLevel(InputDepthSampler, pixel, 0).g;
	// only return negative if it is positive
	return sign(eyeDepth) == -1.f ? eyeDepth : -eyeDepth;
}

float FetchEyeDepth(float2 pixel, float centerDepth) {
	float2 tap = InputDepth.SampleLevel(InputDepthSampler, pixel, 0).xy;
	if (tap.x >= 1.f) {
		return centerDepth;
	}

	float currentDepth = sign(tap.y) == -1.f ? tap.y : -tap.y;
	return lerp(centerDepth, currentDepth, exp(-sqr((currentDepth - centerDepth) * g_ThresholdRatio)));
}

float FetchProjDepth(float2 pixel) {
    return InputDepth.Load(int3(pixel, 0)).r;
}

#define FILTER_RADIUS 2.f
float CreateIsosurfaceDepth(float2 tex) {
    float2 inPosition = tex * float2(g_ViewportWidth, g_ViewportHeight);
    float2 dx = float2(1.f / g_ViewportWidth, 0.f);
	float2 dy = float2(0.f, 1.f / g_ViewportHeight);

	/*
	| 0 | 1 | 2 |
	+---+---+---+
	| 3 | 4 | 5 |  --> 4 is the center
	+---+---+---+
	| 6 | 7 | 8 |
	*/
	float taps[9];
	taps[4] = FetchEyeDepth(tex);
	taps[0] = FetchEyeDepth(tex - dx - dy, taps[4]);
	taps[1] = FetchEyeDepth(tex - dy, taps[4]);
	taps[2] = FetchEyeDepth(tex + dx - dy, taps[4]);
	taps[3] = FetchEyeDepth(tex - dx, taps[4]);
	taps[5] = FetchEyeDepth(tex + dx, taps[4]);
	taps[6] = FetchEyeDepth(tex - dx + dy, taps[4]);
	taps[7] = FetchEyeDepth(tex + dy, taps[4]);
	taps[8] = FetchEyeDepth(tex + dx + dy, taps[4]);

	float zc = taps[4];
	float zdxp = taps[5];
	float zdxn = taps[3];

	float zdx = 0.5f * (zdxp - zdxn);

	float zdyp = taps[7];
	float zdyn = taps[1];

	float zdy = 0.5f * (zdyp - zdyn);

	float zdx2 = zdxp + zdxn - 2.f * zc;
	float zdy2 = zdyp + zdyn - 2.f * zc;
	
	float zdxpyp = taps[8];
	float zdxnyn = taps[0];
	float zdxpyn = taps[2];
	float zdxnyp = taps[6];

	float zdxy = (zdxpyp + zdxnyn - zdxpyn - zdxnyp) / 4.f;

	float cx = 2.f / (g_ViewportWidth * -g_Projection[0][0]);
	float cy = 2.f / (g_ViewportHeight * -g_Projection[1][1]);

	float d = cy * cy * zdx * zdx + cx * cx * zdy * zdy + cx * cx * cy * cy * zc * zc;

	float ddx = cy * cy * 2.f * zdx * zdx2 + cx * cx * 2.f * zdy * zdxy + cx * cx * cy * cy * 2.f * zc * zdx;
	float ddy = cy * cy * 2.f * zdx * zdxy + cx * cx * 2.f * zdy * zdy2 + cx * cx * cy * cy * 2.f * zc * zdy;

	float ex = 0.5f * zdx * ddx - zdx2 * d;
	float ey = 0.5f * zdy * ddy - zdy2 * d;

	float h = 0.5f * ((cy * ex + cx * ey) / pow(d, 1.5f));

	float final = zc + h * 0.0004f * (1.f + (abs(zdx) + abs(zdy)) * 1000.f);

	return final;
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