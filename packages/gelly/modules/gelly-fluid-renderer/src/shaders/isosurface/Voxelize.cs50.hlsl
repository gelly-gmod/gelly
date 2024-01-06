// The threads are completely arbitrary, but a workgroup size of 64 is favored by some hardware.
[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID) {

}