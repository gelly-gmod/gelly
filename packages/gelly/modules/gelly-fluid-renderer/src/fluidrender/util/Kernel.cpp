#include "fluidrender/util/Kernel.h"

using namespace Gelly::util;

Kernel::Kernel()
	: m_context(nullptr), m_shader(nullptr), m_dispatchSize({0, 0, 0}) {}

void Kernel::Initialize(
	GellyInterfaceVal<IRenderContext> context,
	const uint8_t *shaderBytecode,
	size_t shaderBytecodeSize,
	uint3 dispatchSize
) {
	m_context = context;
	m_shader = m_context->CreateShader(
		shaderBytecode, shaderBytecodeSize, ShaderType::Compute
	);
	m_dispatchSize = dispatchSize;

	// important: reserve will pre-allocate, but we actually want to initialize
	// the vectors with empty values.
	m_inputs.resize(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	m_outputs.resize(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	m_cBuffers.resize(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);

	m_initialized = true;
}

bool Kernel::ValidateInput(const KernelIO &input) {
	if (IsKernelIOBuffer(input)) {
		const auto &buffer = std::get<GellyInterfaceVal<IManagedBuffer>>(input);
		const auto bufferDesc = buffer->GetDesc();

		const bool isValid =
			(bufferDesc.type & BufferType::UNORDERED_ACCESS) > 0 ||
			(bufferDesc.type & BufferType::SHADER_RESOURCE) > 0;

		return isValid;
	}

	if (IsKernelIOTexture(input)) {
		const auto &texture =
			std::get<GellyInterfaceVal<IManagedTexture>>(input);
		const auto textureDesc = texture->GetDesc();

		const bool isValid = textureDesc.access & TextureAccess::READ > 0;

		return isValid;
	}

	return false;
}

bool Kernel::ValidateOutput(const KernelIO &output) {
	if (IsKernelIOBuffer(output)) {
		const auto &buffer =
			std::get<GellyInterfaceVal<IManagedBuffer>>(output);
		const auto bufferDesc = buffer->GetDesc();

		const bool isValid = bufferDesc.type == BufferType::UNORDERED_ACCESS;

		return isValid;
	}

	if (IsKernelIOTexture(output)) {
		const auto &texture =
			std::get<GellyInterfaceVal<IManagedTexture>>(output);
		const auto textureDesc = texture->GetDesc();

		const bool isValid = textureDesc.access & TextureAccess::WRITE > 0;

		return isValid;
	}

	return false;
}

bool Kernel::ValidateCBuffer(const GellyInterfaceVal<IManagedBuffer> &cBuffer) {
	const auto &bufferDesc = cBuffer->GetDesc();
	return bufferDesc.type == BufferType::CONSTANT;
}

void Kernel::BindInput(const KernelIO &input, uint index) {
	if (IsKernelIOBuffer(input)) {
		const auto &buffer = std::get<GellyInterfaceVal<IManagedBuffer>>(input);

		buffer->BindToPipeline(ShaderType::Compute, index);
	} else if (IsKernelIOTexture(input)) {
		const auto &texture =
			std::get<GellyInterfaceVal<IManagedTexture>>(input);

		texture->BindToPipeline(
			TextureBindStage::COMPUTE_SHADER_READ, index, std::nullopt
		);
	} else {
		throw std::logic_error(
			"Kernel::BindInput encountered an unimplemented input"
		);
	}
}

void Kernel::BindOutput(const KernelIO &output, uint index) {
	if (IsKernelIOBuffer(output)) {
		const auto &buffer =
			std::get<GellyInterfaceVal<IManagedBuffer>>(output);

		// Could be more explicit, but if the buffer is set up for unordered
		// access, this will set the UAV.
		buffer->BindToPipeline(ShaderType::Compute, index);
	} else if (IsKernelIOTexture(output)) {
		const auto &texture =
			std::get<GellyInterfaceVal<IManagedTexture>>(output);

		texture->BindToPipeline(
			TextureBindStage::COMPUTE_SHADER_WRITE, index, std::nullopt
		);
	} else {
		throw std::logic_error(
			"Kernel::BindOutput encountered an unimplemented output"
		);
	}
}

void Kernel::BindCBuffer(
	const GellyInterfaceVal<IManagedBuffer> &cBuffer, uint index
) {
	cBuffer->BindToPipeline(ShaderType::Compute, index);
}

void Kernel::SetInput(uint index, KernelIO input) {
	if (!m_initialized) {
		throw std::logic_error(
			"Kernel::SetInput called on an uninitialized kernel"
		);
	}

	if (!ValidateInput(input)) {
		throw std::logic_error("Kernel::SetInput encountered an invalid input");
	}

	m_inputs[index] = input;
}

void Kernel::SetOutput(uint index, KernelIO output) {
	if (!m_initialized) {
		throw std::logic_error(
			"Kernel::SetOutput called on an uninitialized kernel"
		);
	}

	if (!ValidateOutput(output)) {
		throw std::logic_error("Kernel::SetOutput encountered an invalid output"
		);
	}

	m_outputs[index] = output;
}

void Kernel::SetCBuffer(uint index, GellyInterfaceVal<IManagedBuffer> cBuffer) {
	if (!m_initialized) {
		throw std::logic_error(
			"Kernel::SetCBuffer called on an uninitialized kernel"
		);
	}

	if (!ValidateCBuffer(cBuffer)) {
		throw std::logic_error(
			"Kernel::SetCBuffer encountered an invalid cbuffer"
		);
	}

	m_cBuffers[index] = cBuffer;
}

void Kernel::Invoke() {
	if (!m_initialized) {
		throw std::logic_error(
			"Kernel::Invoke called on an uninitialized kernel"
		);
	}

	for (uint i = 0; i < m_inputs.size(); i++) {
		if (!IsKernelIOEmpty(m_inputs[i])) {
			BindInput(m_inputs[i], i);
		}
	}

	for (uint i = 0; i < m_outputs.size(); i++) {
		if (!IsKernelIOEmpty(m_outputs[i])) {
			BindOutput(m_outputs[i], i);
		}
	}

	for (uint i = 0; i < m_cBuffers.size(); i++) {
		if (m_cBuffers[i]) {
			BindCBuffer(m_cBuffers[i], i);
		}
	}

	m_shader->Bind();
	m_context->Dispatch(m_dispatchSize.x, m_dispatchSize.y, m_dispatchSize.z);
	m_context->ResetPipeline();
}
