#include "ShellCmd.h"

#include <stdexcept>

ShellCmd::ShellCmd(
	const std::string &commandLine, const std::string &workingDirectory
) {
	STARTUPINFO startupInfo = {};
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	// we null this out so that the child process doesn't inherit our console
	startupInfo.hStdOutput = nullptr;
	startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	if (commandLine.size() > 1024) {
		throw std::runtime_error(
			"ShellCmd::ShellCmd() encountered a command line that was too long"
		);
	}

	char commandLineBuffer[1024];
	strcpy_s(commandLineBuffer, commandLine.c_str());

	if (const auto result = CreateProcess(
			nullptr,
			commandLineBuffer,
			nullptr,
			nullptr,
			TRUE,
			0,
			nullptr,
			workingDirectory.c_str(),
			&startupInfo,
			&processInfo
		);
		!result) {
		printf("CreateProcess failed (%lu).\n", GetLastError());
		isValid = false;
		return;
	}

	isValid = true;
}

ShellCmd::~ShellCmd() {
	if (isValid) {
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
}

bool ShellCmd::IsValid() const noexcept { return isValid; }

int ShellCmd::WaitForCompletion() const {
	if (!isValid) {
		return -1;
	}

	WaitForSingleObject(processInfo.hProcess, INFINITE);

	DWORD exitCode;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	return static_cast<int>(exitCode);
}
