#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"
#include "util/EyeToProjDepth.hlsli"
#include "util/IsUnderwater.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

Texture2D InputNormal : register(t1);
SamplerState InputNormalSampler : register(s1);

static const float INVALID_EYE_DEPTH_EPSILON = 0.001f;
static const float NORMAL_MIP_LEVEL = 8.f;
static const float FILTER_RADIUS = 4.f;

struct PS_OUTPUT {
    float4 FilteredNormal : SV_Target0;
};

float4 FetchNormal(float2 pixel, float eyeDepth, out float mipLevel) {
	// In order to increase our kernel's footprint cheaply, we can compute the size of the pixel in world space to determine the mip level to sample from
	// This is a cheap way to increase the kernel's footprint without having to do any expensive calculations
	// Anyways, to facilitate this we could accurately unproject the pixel and calculate something something derivative, but that's expensive

	// This term cheaply accounts for higher depth discontinuities across
	// high radius particles
	float radiusAdjustmentTerm = 0.2f * g_ParticleRadius;
	mipLevel = 8 - 2.f * log2((0.09f * abs(eyeDepth)) + 0.0001f) + radiusAdjustmentTerm;
	mipLevel = clamp(mipLevel, 0, NORMAL_MIP_LEVEL);

	float2 uv = pixel * g_InvViewport;
    // improve the incoming bilinear sample
    uv = uv * float2(g_ViewportWidth, g_ViewportHeight) + 0.5f;
    float2 iuv = floor(uv);
    float2 fuv = frac(uv);
    uv = iuv + fuv * fuv * (3.0f - 2.0f * fuv);
    uv = (uv - 0.5f) / float2(g_ViewportWidth, g_ViewportHeight);

    return InputNormal.SampleLevel(InputNormalSampler, uv, mipLevel);
}

inline bool IsDepthInvalid(float depth) {
	return depth >= INVALID_EYE_DEPTH_EPSILON;
}

inline bool IsNormalAllZero(float3 normal) {
	return dot(normal, normal) == 0.f;
}

inline bool IsNormalAllOne(float3 normal) {
	return dot(normal, normal) == 3.f;
}

float IGN_PassCorrelated(float pixelX, float pixelY, int passIndex) {
    passIndex = passIndex % 64;
    float x = float(pixelX) + 5.588238f * float(passIndex);
    float y = float(pixelY) + 5.588238f * float(passIndex);

    return fmod(52.9829189f * fmod(0.06711056f*x + 0.00583715f*y, 1.0f), 1.0f);
}

// Shorthand for dispersing a filter point along the filter radius according to pass-correlated noise
float F(float depth, float v, float2 coord, float passBias) {
    float effectiveFilterRadius = FILTER_RADIUS;
    // Reduce filter radius as pixel footprint of the fluid reduces
    float radiusCompensation = min(2.f, 10.f / g_ParticleRadius);
    effectiveFilterRadius -= (0.001f * -depth - 0.5f) * radiusCompensation; // arbitrary factor
    float filterMinimum = 0.5f * smoothstep(0, 500, -depth); // Forces the filter to never reach zero when viewed from afar, but allows it when fluid is viewed up close.

    effectiveFilterRadius = clamp(effectiveFilterRadius, filterMinimum, FILTER_RADIUS);

    return v * (IGN_PassCorrelated(coord.x, coord.y, passBias + g_SmoothingPassIndex) * effectiveFilterRadius);
}

float4 CreateIsosurfaceNormals(float2 tex) {
    float2 centerPixel = tex * float2(g_ViewportWidth, g_ViewportHeight);
    
    float kernel[9] = {
        0.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		0.f, 0.f, 0.f
    };
	
	// to reduce L2 accesses/vram pressure we store the low-quality eye depth in the alpha component of the normals,
	// but since we also typically rely on the depth for mipmapping we'll first fetch the *center* depth and then use that to fetch the normals

	float4 centerNormal = InputNormal.SampleLevel(InputNormalSampler, tex, 0);
	float centerEyeDepth = centerNormal.a;

	// we use this to track the reliability of the center eye depth (it quickly turns into mush once you go past mip level 6 and causes severe artifacts)
	float mips[9] = {
		0.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		0.f, 0.f, 0.f
	};

    float4 normalTaps[9] = {
        FetchNormal(centerPixel + float2(F(centerEyeDepth, -1, centerPixel, 1), F(centerEyeDepth, -1, centerPixel, 1)), centerEyeDepth, mips[0]),
        FetchNormal(centerPixel + float2(F(centerEyeDepth,  0, centerPixel, 1), F(centerEyeDepth, -1, centerPixel, 1)), centerEyeDepth, mips[1]),
        FetchNormal(centerPixel + float2(F(centerEyeDepth,  1, centerPixel, 1), F(centerEyeDepth, -1, centerPixel, 1)), centerEyeDepth, mips[2]),
        FetchNormal(centerPixel + float2(F(centerEyeDepth, -1, centerPixel, 1), F(centerEyeDepth, 0, centerPixel, 1)), centerEyeDepth, mips[3]),
        centerNormal,
        FetchNormal(centerPixel + float2(F(centerEyeDepth,  1, centerPixel, 1), F(centerEyeDepth, 0, centerPixel, 1)), centerEyeDepth, mips[5]),
        FetchNormal(centerPixel + float2(F(centerEyeDepth, -1, centerPixel, 1), F(centerEyeDepth, 1, centerPixel, 1)), centerEyeDepth, mips[6]),
        FetchNormal(centerPixel + float2(F(centerEyeDepth,  0, centerPixel, 1), F(centerEyeDepth, 1, centerPixel, 1)), centerEyeDepth, mips[7]),
        FetchNormal(centerPixel + float2(F(centerEyeDepth,  1, centerPixel, 1), F(centerEyeDepth, 1, centerPixel, 1)), centerEyeDepth, mips[8])
    };

    [loop]
    for (int i = 0; i < 9; i++) {
        // our pipeline initializes every invalid depth to D3D11_FLOAT32_MAX so this is how we can check for invalid depth
        // fun thing is that we don't actually need to even have a special case for this, as the normal will be discarded anyway
		// since FLOAT32_MAX - <average depth> is always going to be a large number, and it will just be discarded by the kernel

        if (IsNormalAllZero(normalTaps[i].xyz) || IsNormalAllOne(normalTaps[i].xyz)) {
            kernel[i] = 0.f; // discard this kernel value if the normal is invalid
        }
    }

    [loop]
    for (int j = 0; j < 9; j++) { // fyi: different variable name to avoid shadowing since hlsl is unrolling every loop
		if (mips[j] >= (NORMAL_MIP_LEVEL - 5.f)) {
			kernel[j] = 1.f / 9.f;
			continue;
		}

		kernel[j] = abs(normalTaps[j].a - centerEyeDepth) * 120.f;
		// scale the difference to be between 0 and 1
		kernel[j] /= -centerEyeDepth; // flip since we're in negative space
		kernel[j] = saturate(kernel[j]);

		// invert the value so that the maximum value is 1 (which semantically means that the closer the value is to 1, the more we want to retain it)
		kernel[j] = 1.f - kernel[j];
        kernel[j] *= 1 / 9.f; // keep the sum of the kernel to 1 to avoid biasing the normal
    }

    float3 normal = float3(0, 0, 0);
    [loop]
    for (int k = 0; k < 9; k++) {
        normal += normalTaps[k].xyz * kernel[k];
    }

    normal = normalize(normal);
    return float4(normal, centerEyeDepth);
}


PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float2 depth = InputDepth.SampleLevel(InputDepthSampler, input.Tex, 0).xy;
	float projDepth = depth.x;
    if (projDepth >= 1.f) {
        discard;
    }

    if (g_SmoothingPassIndex == -1) {
        // no smoothness, just output old normals
        output.FilteredNormal = InputNormal.SampleLevel(InputNormalSampler, input.Tex, 0);
        return output;
    }

	// there's really no point in filtering the depth if it's underwater
	if (IsProjDepthUnderwater(projDepth)) {
		output.FilteredNormal = float4(0, 0, 0, 1);
		return output;
	}

    float4 normal = CreateIsosurfaceNormals(input.Tex);
    if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z)) {
        // return the original normal if the new one is invalid
        output.FilteredNormal = InputNormal.SampleLevel(InputNormalSampler, input.Tex, 0);
        return output;
    }

    output.FilteredNormal = normal;
    return output;
}