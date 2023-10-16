RWBuffer<float4> positions;

uint CollapseIndexTo1D(uint3 index) {
    return index.x + index.y * 3 + index.z * 9;
}

[numthreads(3, 3, 3)]
void main(uint3 threadID : SV_DispatchThreadID) {
    // For demo purposes, just set the position to the thread ID multiplied by 0.01

    uint index = CollapseIndexTo1D(threadID);
    uint positionsLength;
    positions.GetDimensions(positionsLength);
    if (index >= positionsLength) {
        return;
    }
    positions[index] = float4(threadID, 1);
}