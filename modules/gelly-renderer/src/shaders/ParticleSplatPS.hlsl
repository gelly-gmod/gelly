struct PS_INPUT {
    float4 position : SV_Position;
    float4 worldPos : worldPos;
    float4 centerPos : centerPos;
};

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_OUTPUT PSMain(PS_INPUT input) {
    PS_OUTPUT output;

    // Get our screen pixel position

    output.color = float4(1.0f, distance(input.position.xy / float2(840, 640), input.centerPos) * 0.01f, 0.0f, 1.0f);

    return output;
}