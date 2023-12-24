#ifndef IMANAGEDDEPTHBUFFER_H
#define IMANAGEDDEPTHBUFFER_H

#include <GellyInterface.h>
#include <GellyInterfaceRef.h>

class IRenderContext;

namespace Gelly {
enum class DepthFormat {
	D24S8,
};

enum class DepthOp {
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
};

enum class DepthBufferResource {
	D3D11_DSV,
};

struct DepthBufferDesc {
	DepthFormat format;
	DepthOp depthOp;
};
}  // namespace Gelly

/**
 * The depth buffer isn't just a texture, but also many other things which is
 * why it is contained in this interface.
 */
gelly_interface IManagedDepthBuffer {
public:
	virtual ~IManagedDepthBuffer() = default;

	virtual void SetDesc(const Gelly::DepthBufferDesc &desc) = 0;
	virtual Gelly::DepthBufferDesc GetDesc() const = 0;

	virtual void AttachToContext(GellyInterfaceVal<IRenderContext> context) = 0;
	virtual void Clear(float depth) = 0;

	virtual void Create() = 0;
	virtual void Destroy() = 0;

	/**
	 * \brief Allows users of this class to request the underlying resources of
	 * the depth buffer. For example, the D3D11 users will want to get the D3D11
	 * depth stencil view for depth testing.
	 * \param resource The resource to request.
	 * \return A pointer to the requested resource. It may be null
	 * if the resource does not exist.
	 */
	virtual void *RequestResource(Gelly::DepthBufferResource resource) = 0;
};

#endif	// IMANAGEDDEPTHBUFFER_H
