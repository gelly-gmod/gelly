struct VS_INPUT {
    float4 Pos : SV_Position;
	uint ID : SV_VertexID;
    float4 AnisotropyQ1 : ANISOTROPY0;
    float4 AnisotropyQ2 : ANISOTROPY1;
    float4 AnisotropyQ3 : ANISOTROPY2;
};

struct VS_OUTPUT {
    float4 Pos : SV_Position;
    float4 Bounds : BOUNDS;
    float4x4 InvQuadric : INVQUADRIC;
    float2 NDCPos : NDCPos;
	float3 Absorption : ABSORPTION;
	float Acceleration : ACCELERATION;
	float Variance : VARIANCE;
};

struct GS_OUTPUT {
    float4 Pos : SV_Position;
    float4x4 InvQuadric : INVQUADRIC;
	float3 Absorption : ABSORPTION;
	float Acceleration : ACCELERATION;
};

struct PS_OUTPUT {
	float4 Absorption : SV_Target0;
	float2 FrontDepth : SV_Target1;
	float4 Thickness : SV_Target2;
	float Depth : SV_Depth;
};
