#ifndef SHADERFILECOMPILER_H
#define SHADERFILECOMPILER_H

#include "ShaderFile.h"

class ShaderFileCompiler {
public:
	using ShaderBytecode = std::vector<uint8_t>;
	using ShaderBytecodePtr = std::shared_ptr<ShaderBytecode>;

private:
	ShaderFile shaderFile;
	ShaderBytecodePtr bytecode;
	bool isDebugEnabled;

	void CompileToBytecode();

public:
	/*
	 * \brief May throw if the shader file could not be compiled.
	 * \note The shader file is moved into the compiler.
	 */
	explicit ShaderFileCompiler(
		ShaderFile shaderFile, bool isDebugEnabled = false
	);

	[[nodiscard]] ShaderBytecodePtr GetBytecode() const;
	[[nodiscard]] const ShaderFile &GetShaderFile() const;
};

#endif	// SHADERFILECOMPILER_H
