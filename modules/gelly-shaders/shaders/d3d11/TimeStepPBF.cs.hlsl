RWBuffer<float4> positions;

[numthreads(27, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID) {
    // For demo purposes, just set the position to the thread ID multiplied by 0.01

    uint index = threadID.x;
    // Generate a random coordinate for the position using the index.

    positions[index] = float4(
        (index % 3) * 3,
        ((index / 3) % 3) * 3,
        (index / 9) * 3,
        1.0f
    );
}