cbuffer cbPerFrame : register(b0) {
    float4x4 matProj;
    float4x4 matView;
    float particleRadius;
    float pad1;
    float pad2;
    float pad3;
};