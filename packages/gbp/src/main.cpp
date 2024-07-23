#include <cstdio>

#include "collect-dll-list-from-argv.h"
#include "create-binary-pack-header.h"
#include "pack-dll.h"

int main(const int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: gbp <comma-delimited list of dlls> <output directory>");
		return 1;
	}

	const auto dlls = CollectDLLsFromArguments(argv);
	if (dlls.empty()) {
		printf("[gbp] No DLLs found in arguments, terminating...\n");
		return 1;
	}

	printf("[gbp] Listing target DLLs:\n");
	for (const auto &dll : dlls) {
		printf("[gbp] DLL: %s\n", dll.c_str());
	}

	printf("[gbp] Targeting %lld DLLs\n", dlls.size());
	printf("[gbp] Output directory: %s\n", argv[2]);

	printf("[gbp] Packing DLLs...\n");

	std::vector<PackedDLL> packedDLLs;
	for (const auto &dll : dlls) {
		const auto packedDll = PackDLL(dll, argv[2]);
		if (packedDll.has_value()) {
			packedDLLs.push_back(packedDll.value());
		}
	}

	for (const auto &packedDll : packedDLLs) {
		printf("[gbp] Packed DLL: %s\n", packedDll.name.c_str());
		printf("[gbp] Output path (source): %s\n", packedDll.cppPath.c_str());
		printf("[gbp] Output path (header): %s\n", packedDll.hPath.c_str());
	}

	printf("[gbp] Successfully packed %lld DLLs\n", packedDLLs.size());
	printf("[gbp] Emitting binary pack header...\n");

	if (!CreateBinaryPackHeaderFile(argv[2], packedDLLs)) {
		printf("[gbp] Failed to emit binary pack header, terminating...\n");
		return 1;
	}

	printf("[gbp] Successfully emitted binary pack header\n");
	printf("[gbp] Done!\n");

	return 0;
}
