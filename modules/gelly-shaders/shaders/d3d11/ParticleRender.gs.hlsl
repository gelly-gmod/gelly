#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"

static const float2 corners[4] = {
    float2(0.0, 1.0), float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0)
};

/*
const mat4 D = mat4(1., 0., 0., 0.,
                    0., 1., 0., 0.,
                    0., 0., 1., 0.,
                    0., 0., 0., -1.);
                    */
static const float4x4 D = float4x4(1.f, 0.f, 0.f, 0.f,
                                   0.f, 1.f, 0.f, 0.f,
                                   0.f, 0.f, 1.f, 0.f,
                                   0.f, 0.f, 0.f, -1.f);

float2 CalculatePerspectiveCorrectSize(float4x4 particleMatrix) {
    /*
    void ComputePointSizeAndPosition(mat4 T)
{
    vec2 xbc;
    vec2 ybc;

    mat4  R = transpose(projectionMatrix * viewMatrix * T);
    float A = dot(R[ 3 ], D * R[ 3 ]);
    float B = -2. * dot(R[ 0 ], D * R[ 3 ]);
    float C = dot(R[ 0 ], D * R[ 0 ]);
    xbc[ 0 ] = (-B - sqrt(B * B - 4. * A * C)) / (2.0 * A);
    xbc[ 1 ] = (-B + sqrt(B * B - 4. * A * C)) / (2.0 * A);
    float sx = abs(xbc[ 0 ] - xbc[ 1 ]) * .5 * u_ScreenWidth;

    A        = dot(R[ 3 ], D * R[ 3 ]);
    B        = -2. * dot(R[ 1 ], D * R[ 3 ]);
    C        = dot(R[ 1 ], D * R[ 1 ]);
    ybc[ 0 ] = (-B - sqrt(B * B - 4. * A * C)) / (2.0 * A);
    ybc[ 1 ] = (-B + sqrt(B * B - 4. * A * C)) / (2.0 * A);
    float sy = abs(ybc[ 0 ] - ybc[ 1 ]) * .5 * u_ScreenHeight;

    float pointSize = ceil(max(sx, sy));
    gl_PointSize = pointSize;
}
*/

    float2 xbc;
    float2 ybc;

    float4x4 R = mul(mul(matProj, matView), particleMatrix);
    R = transpose(R);
    // Setup the coefficients for the equation for getting the width of the bounding box
    float A = dot(R[3], mul(D, R[3]));
    float B = -2.f * dot(R[0], mul(D, R[3]));
    float C = dot(R[0], mul(D, R[0]));
    xbc.x = (-B - sqrt(B * B - 4.f * A * C)) / (2.f * A);
    xbc.y = (-B + sqrt(B * B - 4.f * A * C)) / (2.f * A);
    // Take the distance between the two zeros to get our radius (divide by two also)
    float sx = abs(xbc.x - xbc.y) * 0.5f;

    A = dot(R[3], mul(D, R[3]));
    B = -2.f * dot(R[1], mul(D, R[3]));
    C = dot(R[1], mul(D, R[1]));
    ybc.x = (-B - sqrt(B * B - 4.f * A * C)) / (2.f * A);
    ybc.y = (-B + sqrt(B * B - 4.f * A * C)) / (2.f * A);
    float sy = abs(ybc.x - ybc.y) * 0.5f;

    return float2(max(sx, sy), max(sx, sy));
}

float4x4 ComputeIsotropicMatrix(float3 particlePos) {
    /*             mat4(u_PointRadius, 0, 0, 0,
                  0, u_PointRadius, 0, 0,
                  0, 0, u_PointRadius, 0,
                  v_Position.x, v_Position.y, v_Position.z, 1.0) :
                  */
    return float4x4(
        particleRadius, 0.f, 0.f, 0.f,
        0.f, particleRadius, 0.f, 0.f,
        0.f, 0.f, particleRadius, 0.f,
        particlePos.x, particlePos.y, particlePos.z, 1.f
    );
}

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream) {
    float4x4 isotropicParticleMatrix = ComputeIsotropicMatrix(float3(0, 0, 0));
    float2 pointSizes = CalculatePerspectiveCorrectSize(isotropicParticleMatrix);

    for (int i = 0; i < 4; ++i) {
        GS_OUTPUT output = (GS_OUTPUT)0;
        float2 corner = corners[i];
        float4 viewCorner = input[0].ViewPos;
        viewCorner.xy += (corner - 0.5f) * particleRadius;
        float4 projectedPosition = mul(viewCorner, matProj);

        output.Position = projectedPosition;
        output.Texcoord = float2(corner.x, 1.f - corner.y);
        output.ViewPosition = viewCorner;

        stream.Append(output);
    }
}