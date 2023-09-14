#ifndef GELLY_PERFRAMECBUFFER_H
#define GELLY_PERFRAMECBUFFER_H

#include <directxmath.h>

using namespace DirectX;

// Shared by all techniques.
struct PerFrameCBuffer {
	XMFLOAT2 res;
	XMFLOAT2 padding;  // Required for 16-byte alignment.
	XMFLOAT4X4 projection;
	XMFLOAT4X4 view;
	XMFLOAT4X4 invProj;
	XMFLOAT4X4 invView;
	XMFLOAT3 eye;
	float particleRadius;
};

#endif	// GELLY_PERFRAMECBUFFER_H
