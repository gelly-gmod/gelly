#ifndef GELLY_DATATYPES_H
#define GELLY_DATATYPES_H

// Usually you could use XMVECTOR, but there's no guarantees from FleX about the
// alignment of the data. It's safer to just use a struct.
struct ParticlePoint {
	float x, y, z, w;
};

#endif	// GELLY_DATATYPES_H
