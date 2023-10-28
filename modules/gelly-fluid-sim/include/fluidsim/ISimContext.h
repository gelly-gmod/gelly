#ifndef GELLY_ISIMCONTEXT_H
#define GELLY_ISIMCONTEXT_H

#include <GellyInterface.h>

enum class RenderAPIHandle {
	D3D11Device,
	D3D11DeviceContext,
};

gelly_interface ISimContext {
public:
	// Note: subclasses of this class shouldn't really have anything to destroy
	virtual ~ISimContext() = default;

	virtual void *GetRenderAPIHandle(RenderAPIHandle handle) = 0;
};

#endif	// GELLY_ISIMCONTEXT_H