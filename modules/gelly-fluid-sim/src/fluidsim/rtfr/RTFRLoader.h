#ifndef RTFRLOADER_H
#define RTFRLOADER_H

// Loads information about the RTFR dataset from a given path.
// It also parses the viz_info.txt file to gain information about the dataset.
// It does not actually parse the dataset itself.

#include <GellyDataTypes.h>

#include <filesystem>

using namespace Gelly::DataTypes;

namespace rtfr {
struct DatasetInfo {
	std::string name;
	std::filesystem::path path;

	bool anisotropyPresent;
	uint frameCount;
	float particleRadius;
	uint particleCount;
};

/**
 * \brief Loads information about the RTFR dataset from a given path.
 * \param datasetPath Path to the dataset folder.
 * \return
 */
DatasetInfo LoadDatasetInfo(const std::filesystem::path &datasetPath);

}  // namespace rtfr

#endif	// RTFRLOADER_H
