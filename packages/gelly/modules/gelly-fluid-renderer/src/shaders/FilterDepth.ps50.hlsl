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

struct PS_OUTPUT {
    float4 FilteredNormal : SV_Target0;
};

float FetchEyeDepth(float2 pixel) {
    float eyeDepth = InputDepth.SampleLevel(InputDepthSampler, pixel / float2(g_ViewportWidth, g_ViewportHeight), 0).g;
	// only return negative if it is positive
	return sign(eyeDepth) == -1.f ? eyeDepth : -eyeDepth;
}

float3 FetchNormal(float2 pixel, float eyeDepth) {
	// In order to increase our kernel's footprint cheaply, we can compute the size of the pixel in world space to determine the mip level to sample from
	// This is a cheap way to increase the kernel's footprint without having to do any expensive calculations
	// Anyways, to facilitate this we could accurately unproject the pixel and calculate something something derivative, but that's expensive

	float mipLevel = 8 - 1.3f * log2((0.2f * abs(eyeDepth)) + 0.0001f);
	mipLevel = clamp(mipLevel, 0, NORMAL_MIP_LEVEL);

	float2 uv = pixel / float2(g_ViewportWidth, g_ViewportHeight);
    return InputNormal.SampleLevel(InputNormalSampler, uv, mipLevel).xyz;
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

float3 CreateIsosurfaceNormals(float2 tex) {
    // We compute the normal of the isosurface at the given pixel by sampling the depth and normal textures
    float2 centerPixel = tex * float2(g_ViewportWidth, g_ViewportHeight);

    /*
	3x3 isosurface normal kernel
    [ 0 ] [ 1 ] [ 2 ]
    [ 3 ] [ 4 ] [ 5 ]
    [ 6 ] [ 7 ] [ 8 ]

    We, at filter-time, generate a kernel which remaps the depth values from the maximum being 1/9 to zero.
    This allows us to sample the depth values in a way that is more robust to noise, and properly filters out discontinuities.
	We also retain 50% of the last generation's normal to prevent losing features and valid data which may have too small of a footprint to
	be considered otherwise.
    */

    float kernel[9] = {
        FetchEyeDepth(centerPixel + float2(-1, -1)),
        FetchEyeDepth(centerPixel + float2( 0, -1)),
        FetchEyeDepth(centerPixel + float2( 1, -1)),
        FetchEyeDepth(centerPixel + float2(-1,  0)),
        FetchEyeDepth(centerPixel + float2( 0,  0)),
        FetchEyeDepth(centerPixel + float2( 1,  0)),
        FetchEyeDepth(centerPixel + float2(-1,  1)),
        FetchEyeDepth(centerPixel + float2( 0,  1)),
        FetchEyeDepth(centerPixel + float2( 1,  1))
    };
	
    float3 normalTaps[9] = {
        FetchNormal(centerPixel + float2(-1, -1), kernel[0]),
        FetchNormal(centerPixel + float2( 0, -1), kernel[1]),
        FetchNormal(centerPixel + float2( 1, -1), kernel[2]),
        FetchNormal(centerPixel + float2(-1,  0), kernel[3]),
        FetchNormal(centerPixel + float2( 0,  0), kernel[4]),
        FetchNormal(centerPixel + float2( 1,  0), kernel[5]),
        FetchNormal(centerPixel + float2(-1,  1), kernel[6]),
        FetchNormal(centerPixel + float2( 0,  1), kernel[7]),
        FetchNormal(centerPixel + float2( 1,  1), kernel[8])
    };

    [unroll]
    for (int i = 0; i < 9; i++) {
        // our pipeline initializes every invalid depth to D3D11_FLOAT32_MAX so this is how we can check for invalid depth
        // fun thing is that we don't actually need to even have a special case for this, as the normal will be discarded anyway
		// since FLOAT32_MAX - <average depth> is always going to be a large number, and it will just be discarded by the kernel

        if (IsNormalAllZero(normalTaps[i]) || IsNormalAllOne(normalTaps[i])) {
            kernel[i] = 0.f; // discard this kernel value if the normal is invalid
        }
    }

    float centerKernel = kernel[4];
    [unroll]
    for (int j = 0; j < 9; j++) { // fyi: different variable name to avoid shadowing since hlsl is unrolling every loop
        if (kernel[j] == 0.f) {
            continue;
        }

		kernel[j] = abs(kernel[j] - centerKernel) * 20.f;
		// scale the difference to be between 0 and 1
		kernel[j] /= -centerKernel; // flip since we're in negative space
		kernel[j] = saturate(kernel[j]);

		// invert the value so that the maximum value is 1 (which semantically means that the closer the value is to 1, the more we want to retain it)
		kernel[j] = 1.f - kernel[j];
        kernel[j] *= 1 / 9.f; // keep the sum of the kernel to 1 to avoid biasing the normal
    }

    float3 normal = float3(0, 0, 0);
    [unroll]
    for (int k = 0; k < 9; k++) {
        normal += normalTaps[k] * kernel[k];
    }

    normal = normalize(normal);
    return normal;
}


PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float projDepth = InputDepth.SampleLevel(InputDepthSampler, input.Tex, 0).r;

    if (projDepth >= 1.f) {
        discard;
    }

	// there's really no point in filtering the depth if it's underwater
	if (IsProjDepthUnderwater(projDepth)) {
		output.FilteredNormal = float4(0, 0, 0, 1);
		return output;
	}

    float3 normal = CreateIsosurfaceNormals(input.Tex);
    if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z)) {
        // return the original normal if the new one is invalid
        normal = InputNormal.SampleLevel(InputNormalSampler, input.Tex, 0).xyz;
        output.FilteredNormal = float4(normal, 1.f);
        return output;
    }

    output.FilteredNormal = float4(normal, 1.f);
    return output;
}