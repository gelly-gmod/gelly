#ifndef REPLACE_ALL_OCCURRENCES_H
#define REPLACE_ALL_OCCURRENCES_H

#include <string>

inline auto ReplaceAllOccurrences(
	std::string &target, const std::string &key, const std::string &value
) -> void {
	size_t pos = 0;
	// fairly simple, we just shift the target string by the size of the last
	// replacement to avoid only replacing the first occurrence
	while ((pos = target.find(key, pos)) != std::string::npos) {
		target.replace(pos, key.length(), value);
		pos += value.length();
	}
}

#endif	// REPLACE_ALL_OCCURRENCES_H
