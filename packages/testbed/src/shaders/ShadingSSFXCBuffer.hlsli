struct Light {
    float3 position;
    float power;
    float radius;
    float3 color;
};

cbuffer shadingSsfx : register(b1) {
    Light lights[2];
    float lightCount;
    float pad0;
    float pad1;
    float pad2;
};