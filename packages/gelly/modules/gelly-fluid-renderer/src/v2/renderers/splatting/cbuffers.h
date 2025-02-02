#ifndef CBUFFERS_H
#define CBUFFERS_H

#include <helpers/rendering/constant-buffer.h>

#include <shaders/ComputeAccelerationCBuffer.hlsli>
#include <shaders/FluidRenderCBuffer.hlsli>

namespace gelly::renderer::cbuffer {
using FluidRenderCBuffer = util::ConstantBuffer<FluidRenderCBufferData>;
using ComputeAccelerationCBuffer =
	util::ConstantBuffer<ComputeAccelerationCBufferData>;
}  // namespace gelly::renderer::cbuffer

#endif	// CBUFFERS_H
