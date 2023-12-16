#include "RTFRLoader.h"

#include <fstream>

enum VizInfoTuple {
	// Open for extension if more information is needed. That is why an
	// std::pair is not used.
	PARTICLE_RADIUS = 0,
	PARTICLE_COUNT,
};

enum class VizInfoLine { NUM_FLUID_PARTICLES, FLUID_PARTICLE_RADIUS, UNKNOWN };

using VizInfo = std::tuple<float, uint>;

static VizInfoLine GetVizInfoLine(const std::string &line) {
	if (line == "num_fluid_particles") {
		return VizInfoLine::NUM_FLUID_PARTICLES;
	} else if (line == "fluid_particle_radius") {
		return VizInfoLine::FLUID_PARTICLE_RADIUS;
	} else {
		return VizInfoLine::UNKNOWN;
	}
}

static bool IsVizLineEmpty(const std::string &line) {
	return line.empty() || line[0] == '/';
}

static VizInfo ParseVizInfo(const std::string &vizInfoData) {
	std::stringstream dataStream{vizInfoData};
	std::string currentLine;

	float radius = 0.0f;
	uint particleCount = 0;

	while (std::getline(dataStream, currentLine, '\n')) {
		if (IsVizLineEmpty(currentLine)) {
			continue;
		}

		std::stringstream lineStream{currentLine};
		std::string lineToken;

		std::getline(lineStream, lineToken, ' ');

		switch (GetVizInfoLine(lineToken)) {
			case VizInfoLine::NUM_FLUID_PARTICLES: {
				std::string particleCountToken;
				std::getline(lineStream, particleCountToken, ' ');
				particleCount = std::stoi(particleCountToken);
				break;
			}
			case VizInfoLine::FLUID_PARTICLE_RADIUS: {
				std::getline(lineStream, lineToken, ' ');
				radius = std::stof(lineToken);
				break;
			}
			default: {
				break;
			}
		}
	}

	return std::make_tuple(radius, particleCount);
}

static bool IsDatasetValid(const std::filesystem::path &datasetPath) {
	const bool vizInfoPresent =
		std::filesystem::exists(datasetPath / "viz_info.txt");
	const bool fluidFramePresent =
		std::filesystem::exists(datasetPath / "FluidFrame");

	return std::filesystem::exists(datasetPath) &&
		   std::filesystem::is_directory(datasetPath) && vizInfoPresent &&
		   fluidFramePresent;
}

static std::string GetDatasetName(const std::filesystem::path &datasetPath) {
	return datasetPath.filename().string();
}

static bool IsParticlePositionFile(const std::filesystem::path &filePath) {
	return filePath.extension() == ".pos";
}

static bool IsParticleAnisotropyFile(const std::filesystem::path &filePath) {
	return filePath.extension() == ".ani";
}

static uint FindFrameCount(const std::filesystem::path &datasetPath) {
	uint frameCount = 0;
	const auto &fluidFrameDir = datasetPath / "FluidFrame";
	for (const auto &entry :
		 std::filesystem::directory_iterator(fluidFrameDir)) {
		if (IsParticlePositionFile(entry.path())) {
			frameCount++;
		}
	}
	return frameCount;
}

static bool IsAnisotropyPresent(const std::filesystem::path &datasetPath) {
	bool anisotropyFramePresent = false;

	const auto &fluidFrameDir = datasetPath / "FluidFrame";
	for (const auto &entry :
		 std::filesystem::directory_iterator(fluidFrameDir)) {
		if (IsParticleAnisotropyFile(entry.path())) {
			anisotropyFramePresent = true;
			break;
		}
	}

	return anisotropyFramePresent;
}

rtfr::DatasetInfo rtfr::LoadDatasetInfo(const std::filesystem::path &datasetPath
) {
	if (!IsDatasetValid(datasetPath)) {
		throw std::runtime_error("Invalid dataset path.");
	}

	std::string vizInfoData;
	{
		const std::ifstream vizInfoFile{datasetPath / "viz_info.txt"};
		std::stringstream vizInfoStream;
		vizInfoStream << vizInfoFile.rdbuf();
		vizInfoData = vizInfoStream.str();
	}

	if (vizInfoData.empty()) {
		throw std::runtime_error("viz_info.txt is empty.");
	}

	const auto vizInfo = ParseVizInfo(vizInfoData);
	return DatasetInfo{
		GetDatasetName(datasetPath),
		IsAnisotropyPresent(datasetPath),
		FindFrameCount(datasetPath),
		std::get<PARTICLE_RADIUS>(vizInfo),
		std::get<PARTICLE_COUNT>(vizInfo),
	};
}
