#ifndef GELLY_ISIMCONTEXT_H
#define GELLY_ISIMCONTEXT_H

#include <GellyInterface.h>

namespace Gelly {
enum class SimContextHandle {
	D3D11_DEVICE,
	D3D11_DEVICE_CONTEXT,
};

enum class SimContextAPI { D3D11 };
}  // namespace Gelly

using namespace Gelly;

gelly_interface ISimContext {
public:
	// Note: subclasses of this class shouldn't really have anything to destroy
	virtual ~ISimContext() = default;

	virtual SimContextAPI GetAPI() = 0;

	/**
	 * \brief Sets a handle for the render API.
	 * \param handle Which handle to set.
	 * \param value Value of said handle,
	 */
	virtual void SetAPIHandle(SimContextHandle handle, void *value) = 0;
	virtual void *GetAPIHandle(SimContextHandle handle) = 0;
};

#endif	// GELLY_ISIMCONTEXT_H