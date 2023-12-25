float EyeToProjDepth(float depth) {
    float projDepth = (-depth * (g_FarPlane + g_NearPlane) / (g_FarPlane - g_NearPlane) - 2.0f * g_FarPlane * g_NearPlane / (g_FarPlane - g_NearPlane)) / -depth;
    projDepth = projDepth * 0.5f + 0.5f;
    return projDepth;
}