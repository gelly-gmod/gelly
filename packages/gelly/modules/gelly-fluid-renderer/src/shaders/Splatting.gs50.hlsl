#include "SplattingStructs.hlsli"
#include "FluidRenderCBuffer.hlsli"

static const float2 corners[4] = {
    float2(0.0, 1.0), float2(0.0, 0.0),
    float2(1.0, 1.0), float2(1.0, 0.0)
};

// cuts out tiny particles that are just gonna be noise in the final image
#define VARIANCE_THRESHOLD 0.7f

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> triStream) {
    GS_OUTPUT output = (GS_OUTPUT)0;
	if (input[0].Variance < VARIANCE_THRESHOLD) {
		return;
	}

    // if we're outside of the NDC cube, we might as well discard
	if (input[0].NDCPos.x < -1.0 || input[0].NDCPos.x > 1.0 || input[0].NDCPos.y < -1.0 || input[0].NDCPos.y > 1.0) {
		return;
	}

    float4 bounds = input[0].Bounds;
    const float4 invQ0 = input[0].InvQ0;
    const float4 invQ1 = input[0].InvQ1;
    const float4 invQ2 = input[0].InvQ2;
    const float4 invQ3 = input[0].InvQ3;

    float xmin = bounds.x;
    float xmax = bounds.y;
    float ymin = bounds.z;
    float ymax = bounds.w;

    output.Pos = float4(xmin, ymax, 0.5f, 1.0f);
    output.InvQ0 = invQ0;
    output.InvQ1 = invQ1;
    output.InvQ2 = invQ2;
    output.InvQ3 = invQ3;
	output.Absorption = input[0].Absorption;
    triStream.Append(output);

    output.Pos = float4(xmin, ymin, 0.5f, 1.0f);
    output.InvQ0 = invQ0;
    output.InvQ1 = invQ1;
    output.InvQ2 = invQ2;
    output.InvQ3 = invQ3;
	output.Absorption = input[0].Absorption;
    triStream.Append(output);

    output.Pos = float4(xmax, ymax, 0.5f, 1.0f);
    output.InvQ0 = invQ0;
    output.InvQ1 = invQ1;
    output.InvQ2 = invQ2;
    output.InvQ3 = invQ3;
	output.Absorption = input[0].Absorption;
    triStream.Append(output);

    output.Pos = float4(xmax, ymin, 0.5f, 1.0f);
    output.InvQ0 = invQ0;
    output.InvQ1 = invQ1;
    output.InvQ2 = invQ2;
    output.InvQ3 = invQ3;
	output.Absorption = input[0].Absorption;
    triStream.Append(output);
}