#ifndef GELLY_COMPOSITE_H
#define GELLY_COMPOSITE_H

#include "Pass.h"

class Composite : Pass {
public:
	Composite(IDirect3DDevice9 *device);
	~Composite() = default;

	void Render(PassResources *resources) override;
};

#endif	// GELLY_COMPOSITE_H
