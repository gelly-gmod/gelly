#ifndef CBUFFERS_H
#define CBUFFERS_H

#include <helpers/rendering/constant-buffer.h>

#include <shaders/FluidRenderCBuffer.hlsli>

namespace gelly::renderer::cbuffer {
using FluidRenderCBuffer = util::ConstantBuffer<FluidRenderCBufferData>;
}

#endif	// CBUFFERS_H
