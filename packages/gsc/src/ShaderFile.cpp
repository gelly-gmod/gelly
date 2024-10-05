#include "ShaderFile.h"

#include <algorithm>
#include <fstream>
#include <utility>

void ShaderFile::LoadSource() {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open shader file");
	}

	file.seekg(0, std::ios::end);
	auto size = file.tellg();
	file.seekg(0, std::ios::beg);

	source = std::make_shared<ShaderSource>();
	source->resize(size);
	file.read(source->data(), size);
}

void ShaderFile::ComputeFriendlyNameAndProfile() {
	// The friendly name is the name without any extension,
	// and with the shader type appended as "PS", "VS", "GS", etc.
	// So for example, if the path is
	// "./shaders/NDCQuad.vs50.hlsl", the friendly name will be "NDCQuadVS".

	std::string name = path.filename().string();

	if (const auto dotPos = name.find_last_of('.');
		dotPos != std::string::npos) {
		name = name.substr(0, dotPos);
	} else {
		throw std::runtime_error("Shader file name has no extension");
	}

	std::string shaderType;
	if (const auto shaderDotPos = name.find_first_of('.');
		shaderDotPos != std::string::npos) {
		shaderType = name.substr(shaderDotPos + 1);
		name = name.substr(0, shaderDotPos);
	} else {
		throw std::runtime_error("Shader file name has no shader type");
	}

	std::string shaderTypeString;

	switch (shaderType[0]) {
		case 'v':
			shaderTypeString = "VS";
			profile = ShaderProfile::VS;
			break;
		case 'p':
			shaderTypeString = "PS";
			profile = ShaderProfile::PS;
			break;
		case 'g':
			shaderTypeString = "GS";
			profile = ShaderProfile::GS;
			break;
		case 'c':
			shaderTypeString = "CS";
			profile = ShaderProfile::CS;
			break;
		default:
			throw std::runtime_error("Unknown shader type");
			break;
	}

	if (std::find(shaderType.begin(), shaderType.end(), '5') !=
		shaderType.end()) {
		model = SM_5;
	} else if (std::find(shaderType.begin(), shaderType.end(), '3') !=
			   shaderType.end()) {
		model = SM_3;
	} else {
		throw std::runtime_error("Unknown shader model");
	}

	friendlyName = name + shaderTypeString;
}

ShaderFile::ShaderFile(fs::path path, bool debug) :
	path(std::move(path)), profile(ShaderProfile::VS), debug(debug) {
	LoadSource();
	ComputeFriendlyNameAndProfile();
}

ShaderFile::ShaderSourcePtr ShaderFile::GetSource() const { return source; }
const fs::path &ShaderFile::GetPath() const { return path; }
const std::string &ShaderFile::GetFriendlyName() const { return friendlyName; }
ShaderProfile ShaderFile::GetProfile() const { return profile; }
ShaderModel ShaderFile::GetModel() const { return model; }
bool ShaderFile::IsDebugEnabled() const { return debug; }