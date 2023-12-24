#include "detail/d3d11/ComputeProgram.h"

#include "detail/d3d11/Shaders.h"

namespace d3d11 {
ComputeProgram::ComputeProgram(
	ID3D11Device *device,
	const char *programName,
	const char *entryPoint,
	const char *programSource
) {
	ShaderCompileOptions options = {};
	options.device = device;
	options.shader.buffer = (void *)programSource;
	options.shader.name = programName;
	options.shader.entryPoint = entryPoint;
	options.shader.size = strlen(programSource);

	shader.Attach(compile_compute_shader(options).shader);
}
}  // namespace d3d11