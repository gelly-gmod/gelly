#include "FrameParser.h"

#include <fstream>

using namespace rtfr;

FrameHeader rtfr::LoadFrameInfo(const std::filesystem::path &framePath) {
	// All we really need to do is read the header off of the file as it
	// always starts at the beginning of the file.
	std::ifstream frameFile{framePath, std::ios::binary};
	if (!frameFile.is_open()) {
		throw std::runtime_error("Could not open frame file.");
	}

	FrameHeader header = {};
	frameFile.read(reinterpret_cast<char *>(&header), sizeof(FrameHeader));

	return header;
}

void rtfr::LoadFrameData(
	const FrameHeader &header,
	const std::filesystem::path &framePath,
	byte *destinationPtr
) {
	std::ifstream frameFile{framePath, std::ios::binary};
	if (!frameFile.is_open()) {
		throw std::runtime_error("Could not open frame file.");
	}

	// This time we're reading the whole file.
	const std::streamsize frameSize = header.particleCount * PARTICLE_POS_SIZE;

	frameFile.seekg(sizeof(FrameHeader));  // Skip the header.
	frameFile.read(reinterpret_cast<char *>(destinationPtr), frameSize);
}
