#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"
#include "util/EyeToProjDepth.hlsli"
#include "util/IsUnderwater.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

Texture2D InputNormal : register(t1);
SamplerState InputNormalSampler : register(s1);

static const float INVALID_EYE_DEPTH_EPSILON = 0.001f;
static const float RETAINMENT_PERCENTAGE = 0.5f; // retain 50% of last generation's normal

struct PS_OUTPUT {
    float4 Color : SV_Target0;
};

float FetchEyeDepth(float2 pixel) {
    float eyeDepth = InputDepth.SampleLevel(InputDepthSampler, pixel / float2(g_ViewportWidth, g_ViewportHeight), 0).g;
	// only return negative if it is positive
	return sign(eyeDepth) == -1.f ? eyeDepth : -eyeDepth;
}

float FetchProjDepth(float2 pixel) {
    return InputDepth.Load(int3(pixel, 0)).r;
}

float3 FetchNormal(float2 pixel) {
    return InputNormal.SampleLevel(InputNormalSampler, pixel / float2(g_ViewportWidth, g_ViewportHeight), 0).xyz;
}

float3 CreateIsosurfaceNormals(float2 tex) {
    // We compute the normal of the isosurface at the given pixel by sampling the depth and normal textures
    float2 centerPixel = tex * float2(g_ViewportWidth, g_ViewportHeight);
    /*
    [ 0 ] [ 1 ] [ 2 ]
    [ 3 ] [ 4 ] [ 5 ]
    [ 6 ] [ 7 ] [ 8 ]

    We, at filter-time, generate a kernel which remaps the depth values from the maximum being 1/9 to zero.
    This allows us to sample the depth values in a way that is more robust to noise, and properly filters out discontinuities.
    */
    float3 normalTaps[9] = {
        FetchNormal(centerPixel + float2(-1, -1)),
        FetchNormal(centerPixel + float2( 0, -1)),
        FetchNormal(centerPixel + float2( 1, -1)),
        FetchNormal(centerPixel + float2(-1,  0)),
        FetchNormal(centerPixel + float2( 0,  0)),
        FetchNormal(centerPixel + float2( 1,  0)),
        FetchNormal(centerPixel + float2(-1,  1)),
        FetchNormal(centerPixel + float2( 0,  1)),
        FetchNormal(centerPixel + float2( 1,  1))
    };

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

    [unroll]
    for (int i = 0; i < 9; i++) {
        // our pipeline initializes every invalid depth to D3D11_FLOAT32_MAX so this is how we can check for invalid depth
        if (kernel[i] > INVALID_EYE_DEPTH_EPSILON) {
            kernel[i] = kernel[4]; // center pixel
            // we'll also do symmetric balancing for the kernel
            kernel[9 - i] = kernel[4]; // center pixel
        }

        // 0 * 0 = 0, so we can use this to check if the normal is invalid
        // and (1*1+1*1+1*1) = 3, so we can use this to check if the normal is invalid too
        if (dot(normalTaps[i], normalTaps[i]) == 0.f || dot(normalTaps[i], normalTaps[i]) == 3.f) {
            kernel[i] = 0.f; // discard this kernel value if the normal is invalid
        }
    }

    float centerKernel = kernel[4];
    [unroll]
    for (int i = 0; i < 9; i++) {
        if (kernel[i] == 0.f) {
            continue;
        }

        kernel[i] /= centerKernel; // normalize the kernel
        // We want to preserve edges and curves as much as possible, so we will nudge the kernel values depending on the normals
        kernel[i] *= 1 / 9.f;
    }

    float3 normal = float3(0, 0, 0);
    [unroll]
    for (int i = 0; i < 9; i++) {
        normal += normalTaps[i] * kernel[i];
    }

    normal = normalize(normal);
    normal = lerp(normalTaps[4], normal, RETAINMENT_PERCENTAGE);
    return normal;
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

    float3 normal = CreateIsosurfaceNormals(input.Tex);
    if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z)) {
        // return the original normal if the new one is invalid
        normal = FetchNormal(input.Tex);
        output.Color = float4(normal, 1.f);
        return output;
    }

    output.Color = float4(normal, 1.f);
    return output;
}