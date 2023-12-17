#ifndef FRAMEPARSER_H
#define FRAMEPARSER_H

#include <GellyDataTypes.h>

#include <filesystem>
using namespace Gelly::DataTypes;

namespace rtfr {
constexpr uint PARTICLE_POS_SIZE = sizeof(float) * 3;

struct FrameHeader {
	// We use uint4 to explicitly state that there are four bytes in the uint,
	// it's important for file reading.
	uint4 particleCount;
	float particleRadius;
};

/**
 * \note Will throw an exception if the file is not found, but this is
 * relatively cheap as it will purposely never load the full file as they are
 * usually huge.
 * \param framePath Location of the frame file.
 * \return Information about the frame.
 */
FrameHeader LoadFrameInfo(const std::filesystem::path &framePath);

/**
 * \brief Loads the particle positions from the frame file.
 * \param header Header of the frame.
 * \param framePath Path to the frame file.
 * \param destinationPtr Pointer to the address where the frame data will be
 * dumped. This is expected to be a memory block capable of handling 3 floats *
 * particle count.
 */
void LoadFrameData(
	const FrameHeader &header,
	const std::filesystem::path &framePath,
	byte *destinationPtr
);

/**
 * \brief Returns the size of the frame in bytes.
 * \param header Header of the frame to get the size of.
 * \return The size of the frame in bytes.
 */
uint GetFrameSize(const FrameHeader &header);

}  // namespace rtfr

#endif	// FRAMEPARSER_H
