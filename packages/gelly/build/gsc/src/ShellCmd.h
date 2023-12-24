#ifndef SHELLCMD_H
#define SHELLCMD_H
#include <windows.h>

#include <string>

class ShellCmd {
private:
	PROCESS_INFORMATION processInfo{};
	bool isValid;

public:
	ShellCmd(
		const std::string &commandLine, const std::string &workingDirectory
	);

	~ShellCmd();

	/**
	 * \brief Returns the validity of the command. If false, this can mean many
	 * things, but usually that the process could not be created.
	 * \return Boolean, if true then the command is valid.
	 */
	[[nodiscard]] bool IsValid() const noexcept;

	/**
	 * \brief Waits for the process to finish.
	 * \return The exit code of the process.
	 */
	[[nodiscard]] int WaitForCompletion() const;
};

#endif	// SHELLCMD_H
