cbuffer fluidRender : register(b0) {
    float4x4 g_View;
    float4x4 g_Projection;
    float4x4 g_InverseView;
    float4x4 g_InverseProjection;

    float g_ViewportWidth;
    float g_ViewportHeight;
    float g_ThresholdRatio;
    float g_ParticleRadius;
};