// The threads here basically mean that we operate on 4x4x4 chunks of the voxel grid at a time. (noting that 64 = 4^3)
[numthreads(4, 4, 4)]
void main(uint3 threadID : SV_DispatchThreadID) {

}