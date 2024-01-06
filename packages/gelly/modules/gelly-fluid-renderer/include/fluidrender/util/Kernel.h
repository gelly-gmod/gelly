#ifndef KERNEL_H
#define KERNEL_H

#include <variant>

#include "fluidrender/IManagedShader.h"
#include "fluidrender/IRenderContext.h"

namespace Gelly::util {
using KernelIO = std::variant<
	std::monostate,
	GellyInterfaceVal<IManagedBuffer>,
	GellyInterfaceVal<IManagedTexture>>;

enum KernelIOIndex : uint { EMPTY = 0, BUFFER = 1, TEXTURE = 2 };

inline bool IsKernelIOEmpty(const KernelIO &io) {
	return io.index() == KernelIOIndex::EMPTY;
}

inline bool IsKernelIOBuffer(const KernelIO &io) {
	return io.index() == KernelIOIndex::BUFFER;
}

inline bool IsKernelIOTexture(const KernelIO &io) {
	return io.index() == KernelIOIndex::TEXTURE;
}

struct uint3 {
	uint x;
	uint y;
	uint z;
};

/**
 * \brief A utility class for managing compute shaders. This class operates on
 * the basis of inputs and outputs, each compute shader may have multiple inputs
 * and outputs. Each input and output is a buffer or texture. Both are checked
 * for validity before being used.
 *
 * \code{.cpp}
 * Kernel myKernel{context, shaderBytecode, shaderBytecodeSize, {1, 1, 1}};
 * myKernel.SetInput(0, myInputBuffer);
 * myKernel.SetOutput(0, myOutputBuffer);
 * myKernel.SetCBuffers(0, myCBuffer);
 *
 * // somewhere down the line
 *
 * myKernel.Invoke();
 *
 * // synchronize with the GPU
 * context->SubmitWork();
 * \endcode
 */
class Kernel {
private:
	GellyInterfaceVal<IRenderContext> m_context;
	GellyInterfaceVal<IManagedShader> m_shader;
	std::vector<KernelIO> m_inputs;
	std::vector<KernelIO> m_outputs;
	std::vector<GellyInterfaceVal<IManagedBuffer>> m_cBuffers;
	uint3 m_dispatchSize;

	bool ValidateInput(const KernelIO &input);
	bool ValidateOutput(const KernelIO &output);
	bool ValidateCBuffer(const GellyInterfaceVal<IManagedBuffer> &cBuffer);

	void BindInput(const KernelIO &input, uint index);
	void BindOutput(const KernelIO &output, uint index);
	void BindCBuffer(
		const GellyInterfaceVal<IManagedBuffer> &cBuffer, uint index
	);

	bool m_initialized = false;

public:
	Kernel();

	void Initialize(
		GellyInterfaceVal<IRenderContext> context,
		const uint8_t *shaderBytecode,
		size_t shaderBytecodeSize,
		uint3 dispatchSize
	);

	void SetInput(uint index, KernelIO input);
	void SetOutput(uint index, KernelIO output);
	void SetCBuffer(uint index, GellyInterfaceVal<IManagedBuffer> cBuffer);

	void Invoke();
};
}  // namespace Gelly::util
#endif	// KERNEL_H
