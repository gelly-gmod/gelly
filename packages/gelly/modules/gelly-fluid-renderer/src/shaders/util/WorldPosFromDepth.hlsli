float3 WorldPosFromDepthF(float2 tex, float depth) {
    depth = EyeToProjDepth(depth);
    float4 pos = float4(tex.x * 2.0f - 1.0f, (1.0f - tex.y) * 2.0f - 1.0f, depth, 1.0f);
    pos = mul(g_InverseProjection, pos);
    pos = mul(g_InverseView, pos);
    pos.xyz /= pos.w;
    return pos.xyz;
}

float3 WorldPosFromProjDepthF(float2 tex, float depth) {
    float4 pos = float4(tex.x * 2.0f - 1.0f, (1.0f - tex.y) * 2.0f - 1.0f, depth, 1.0f);
    pos = mul(g_InverseProjection, pos);
    pos = mul(g_InverseView, pos);
    pos.xyz /= pos.w;
    return pos.xyz;
}