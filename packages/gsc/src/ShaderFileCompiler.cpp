#include "ShaderFileCompiler.h"

#include <fstream>

#include "ShellCmd.h"

void ShaderFileCompiler::CompileToBytecode() {
	std::string fxcCommand = "fxc.exe /T ";
	std::string fxcProfile;

	switch (shaderFile.GetProfile()) {
		case ShaderProfile::VS:
			fxcProfile = "vs";
			break;
		case ShaderProfile::PS:
			fxcProfile = "ps";
			break;
		case ShaderProfile::GS:
			fxcProfile = "gs";
			break;
		default:
			throw std::runtime_error("Unknown shader profile");
			break;
	}

	switch (shaderFile.GetModel()) {
		case SM_5:
			fxcProfile += "_5_0";
			break;
		case SM_3:
			fxcProfile += "_3_0";
			break;
		default:
			throw std::runtime_error("Unknown shader model");
			break;
	}

	fxcCommand += fxcProfile;
	fxcCommand += " /E main /Fo";

	auto tempPath = fs::temp_directory_path();
	tempPath /= shaderFile.GetFriendlyName() + ".dxbc";
	fxcCommand += tempPath.string();
	fxcCommand += " ";
	fxcCommand += shaderFile.GetPath().string();

	const ShellCmd compileCmd{fxcCommand, fs::current_path().string()};
	if (!compileCmd.IsValid()) {
		// Could mean that the user doesn't have the DirectX SDK installed.
		// So we can make a nice error message.
		std::string errorMessage = "Could not compile shader file ";
		errorMessage += shaderFile.GetPath().string();
		errorMessage += ".\n";
		errorMessage +=
			"Make sure that the DirectX SDK is installed and that fxc.exe is "
			"in your PATH.";

		throw std::runtime_error(errorMessage);
	}

	if (const auto exitCode = compileCmd.WaitForCompletion(); exitCode != 0) {
		std::string errorMessage = "Could not compile shader file ";
		errorMessage += shaderFile.GetPath().string();
		errorMessage += ".\n";
		errorMessage += "fxc.exe returned exit code ";
		errorMessage += std::to_string(exitCode);

		throw std::runtime_error(errorMessage);
	}

	// Read the compiled shader file.
	std::ifstream compiledShaderFile{tempPath, std::ios::binary};
	if (!compiledShaderFile.is_open()) {
		std::string errorMessage = "Could not open compiled shader file ";
		errorMessage += tempPath.string();

		throw std::runtime_error(errorMessage);
	}

	compiledShaderFile.seekg(0, std::ios::end);
	auto fileSize = compiledShaderFile.tellg();
	compiledShaderFile.seekg(0, std::ios::beg);

	bytecode = std::make_shared<ShaderBytecode>();
	bytecode->resize(fileSize);

	compiledShaderFile.read(
		reinterpret_cast<char *>(bytecode->data()), fileSize
	);

	// The user's operating system should clean up the temporary file.
}

ShaderFileCompiler::ShaderFileCompiler(ShaderFile shaderFile)
	: shaderFile{std::move(shaderFile)} {
	CompileToBytecode();
}

ShaderFileCompiler::ShaderBytecodePtr ShaderFileCompiler::GetBytecode() const {
	return bytecode;
}

const ShaderFile &ShaderFileCompiler::GetShaderFile() const {
	return shaderFile;
}
