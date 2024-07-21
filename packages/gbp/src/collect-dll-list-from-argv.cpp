#include "collect-dll-list-from-argv.h"

auto CollectDLLsFromArguments(char *argv[]) -> std::vector<std::string> {
	char *dllList = argv[1];
	// comma-delimited list of dlls
	std::vector<std::string> dlls;
	std::string currentDll;

	for (size_t i = 0; dllList[i] != '\0'; i++) {
		if (dllList[i] == ',') {
			dlls.push_back(currentDll);
			currentDll.clear();
		} else {
			currentDll.push_back(dllList[i]);
		}
	}

	return std::move(dlls);
}
