#ifndef SHADERFILE_H
#define SHADERFILE_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

enum class ShaderProfile {
	PS,
	VS,
};

class ShaderFile {
	using ShaderSource = std::string;
	using ShaderSourcePtr = std::shared_ptr<ShaderSource>;

private:
	fs::path path;
	/**
	 * \brief Used in the C++ glue code generation. Basically it's a nice name
	 * that will work well with any C++ code.
	 */
	std::string friendlyName;

	ShaderSourcePtr source;
	ShaderProfile profile;

	void LoadSource();
	void ComputeFriendlyNameAndProfile();

public:
	/**
	 * \brief May throw if the path is invalid or the file could not be read.
	 * \param path Path to the shader file, either relative or absolute.
	 */
	explicit ShaderFile(fs::path path);

	[[nodiscard]] ShaderSourcePtr GetSource() const;
	[[nodiscard]] const fs::path &GetPath() const;
	[[nodiscard]] const std::string &GetFriendlyName() const;
	[[nodiscard]] ShaderProfile GetProfile() const;
};

#endif	// SHADERFILE_H
