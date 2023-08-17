struct PS_INPUT {
    float4 position : SV_Position;
};

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_OUTPUT PSMain(PS_INPUT input) {
    PS_OUTPUT output;

    output.color = float4(1.0, 0.0, 0.0, 1.0);

    return output;
}