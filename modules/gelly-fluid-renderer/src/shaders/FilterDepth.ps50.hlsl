#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_Target0;
};

static const float g_ParticleRadius = 0.03f;
static const float g_FarPlane = 1000.0f;
static const float g_NearPlane = 0.1f;
static const float g_ThresholdRatio = 5.f;

static const float g_blurWeights[9] = {
    0.0625f, 0.125f, 0.0625f,
    0.125f, 0.25f, 0.125f,
    0.0625f, 0.125f, 0.0625f
};

// float4 BlurDepth(float2 tex) {
//     float4 color = 0.0f;
//     float2 texelSize = 1.0f / float2(g_ViewportWidth, g_ViewportHeight);
    
//     for (int i = 0; i < 9; ++i) {
//         float2 offset = float2(i % 3 - 1, i / 3 - 1) * texelSize;
//         float4 gausSample = InputDepth.Sample(InputDepthSampler, tex + offset);

//         // Also, if the depth is too far away, we're going to assume that it's a discontinuity and discard it
//         if (abs(gausSample.z - InputDepth.Sample(InputDepthSampler, tex).z) > 0.01f) {
//             gausSample = InputDepth.Sample(InputDepthSampler, tex);
//         }

//         gausSample.xyz *= g_blurWeights[i];

//         // if (gausSample.a == 0.f) {
//         //     gausSample = InputDepth.Sample(InputDepthSampler, tex) * g_blurWeights[i];
//         // }
//         // branchless
//         // we just take the original sample if the current sample is null
//         gausSample = lerp(gausSample, InputDepth.Sample(InputDepthSampler, tex) * g_blurWeights[i], 1.f - gausSample.a);
//         color += float4(gausSample.xyz, 1.0f);
//     }

//     return color;
// }

/*
const float thresholdRatio = 5.0;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
vec3 meanCurvature(vec2 pos)
{
    // Width of one pixel
    vec2 dx = vec2(1.0f / u_ScreenWidth, 0.0f);
    vec2 dy = vec2(0.0f, 1.0f / u_ScreenHeight);

    // Central z value
    float zc = texture(u_DepthTex, pos).r - 1.0f;

    float zdxp = texture(u_DepthTex, pos + dx).r - 1.0f;
    float zdxn = texture(u_DepthTex, pos - dx).r - 1.0f;

    float zdx = 0.5f * (zdxp - zdxn);

    float zdyp = texture(u_DepthTex, pos + dy).r - 1.0f;
    float zdyn = texture(u_DepthTex, pos - dy).r - 1.0f;

    float zdy = 0.5f * (zdyp - zdyn);

    // Take second order finite differences
    float zdx2 = zdxp + zdxn - 2.0f * zc;
    float zdy2 = zdyp + zdyn - 2.0f * zc;

    // Second order finite differences, alternating variables
    float zdxpyp = texture(u_DepthTex, pos + dx + dy).r - 1.0f;
    float zdxnyn = texture(u_DepthTex, pos - dx - dy).r - 1.0f;
    float zdxpyn = texture(u_DepthTex, pos + dx - dy).r - 1.0f;
    float zdxnyp = texture(u_DepthTex, pos - dx + dy).r - 1.0f;

    float zdxy = (zdxpyp + zdxnyn - zdxpyn - zdxnyp) / 4.0f;

    if(abs(zdx) > u_ParticleRadius * thresholdRatio) {
        zdx  = 0.0f;
        zdx2 = 0.0f;
    }

    if(abs(zdy) > u_ParticleRadius * thresholdRatio) {
        zdy  = 0.0f;
        zdy2 = 0.0f;
    }

    if(abs(zdxy) > u_ParticleRadius * thresholdRatio) {
        zdxy = 0.0f;
    }

    // Projection transform inversion terms
    float cx = 2.0f / (u_ScreenWidth * -projectionMatrix[0][0]);
    float cy = 2.0f / (u_ScreenHeight * -projectionMatrix[1][1]);

    // Normalization term
    float d = cy * cy * zdx * zdx + cx * cx * zdy * zdy + cx * cx * cy * cy * zc * zc;

    // Derivatives of said term
    float ddx = cy * cy * 2.0f * zdx * zdx2 + cx * cx * 2.0f * zdy * zdxy + cx * cx * cy * cy * 2.0f * zc * zdx;
    float ddy = cy * cy * 2.0f * zdx * zdxy + cx * cx * 2.0f * zdy * zdy2 + cx * cx * cy * cy * 2.0f * zc * zdy;

    // Temporary variables to calculate mean curvature
    float ex = 0.5f * zdx * ddx - zdx2 * d;
    float ey = 0.5f * zdy * ddy - zdy2 * d;

    // Finally, mean curvature
    float h = 0.5f * ((cy * ex + cx * ey) / pow(d, 1.5f));

    return (vec3(zdx, zdy, h));
}

void main()
{
    float particleDepth = texture(u_DepthTex, f_TexCoord).r;
    if(particleDepth < -1000.0f || particleDepth > 0) {
        outDepth = particleDepth;
    } else {
        const float dt   = 0.0003f;
        const float dzt  = 1000.0f;
        vec3        dxyz = meanCurvature(f_TexCoord);

        outDepth = particleDepth + dxyz.z * dt * (1.0f + (abs(dxyz.x) + abs(dxyz.y)) * dzt);
    }
}
*/

float3 GetMeanCurvature(float2 pos) {
    float2 dx = float2(1.0f / g_ViewportWidth, 0.0f);
    float2 dy = float2(0.0f, 1.0f / g_ViewportHeight);

    float zc = InputDepth.Sample(InputDepthSampler, pos).r - 1.0f;

    float zdxp = InputDepth.Sample(InputDepthSampler, pos + dx).r - 1.0f;
    float zdxn = InputDepth.Sample(InputDepthSampler, pos - dx).r - 1.0f;

    float zdx = 0.5f * (zdxp - zdxn);

    float zdyp = InputDepth.Sample(InputDepthSampler, pos + dy).r - 1.0f;
    float zdyn = InputDepth.Sample(InputDepthSampler, pos - dy).r - 1.0f;

    float zdy = 0.5f * (zdyp - zdyn);

    float zdx2 = zdxp + zdxn - 2.0f * zc;
    float zdy2 = zdyp + zdyn - 2.0f * zc;

    float zdxpyp = InputDepth.Sample(InputDepthSampler, pos + dx + dy).r - 1.0f;
    float zdxnyn = InputDepth.Sample(InputDepthSampler, pos - dx - dy).r - 1.0f;
    float zdxpyn = InputDepth.Sample(InputDepthSampler, pos + dx - dy).r - 1.0f;
    float zdxnyp = InputDepth.Sample(InputDepthSampler, pos - dx + dy).r - 1.0f;

    float zdxy = (zdxpyp + zdxnyn - zdxpyn - zdxnyp) / 4.0f;



    float cx = 2.0f / (g_ViewportWidth * -g_Projection[0][0]);
    float cy = 2.0f / (g_ViewportHeight * -g_Projection[1][1]);

    float d = cy * cy * zdx * zdx + cx * cx * zdy * zdy + cx * cx * cy * cy * zc * zc;

    float ddx = cy * cy * 2.0f * zdx * zdx2 + cx * cx * 2.0f * zdy * zdxy + cx * cx * cy * cy * 2.0f * zc * zdx;
    float ddy = cy * cy * 2.0f * zdx * zdxy + cx * cx * 2.0f * zdy * zdy2 + cx * cx * cy * cy * 2.0f * zc * zdy;

    float ex = 0.5f * zdx * ddx - zdx2 * d;
    float ey = 0.5f * zdy * ddy - zdy2 * d;

    float h = 0.5f * ((cy * ex + cx * ey) / pow(d, 1.5f));

    return float3(zdx, zdy, h);
}


PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float4 original = InputDepth.Sample(InputDepthSampler, input.Tex);
    if (original.a == 0.f) {
        discard;
    }

    float dt = 0.0003f;
    float dzt = 1000.0f;
    float3 dxyz = GetMeanCurvature(input.Tex);

    float depth = original.r + dxyz.z * dt * (1.0f + (abs(dxyz.x) + abs(dxyz.y)) * dzt);
    // Convert view depth to projection depth
    //z_ndc = ( -z_eye * (f+n)/(f-n) - 2*f*n/(f-n) ) / -z_eye
    float projDepth = (-depth * (g_FarPlane + g_NearPlane) / (g_FarPlane - g_NearPlane) - 2.0f * g_FarPlane * g_NearPlane / (g_FarPlane - g_NearPlane)) / -depth;

    output.Color = float4(depth, projDepth, depth, 1.0f);
    return output;
}