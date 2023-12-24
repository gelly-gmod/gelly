#ifndef DATASET_H
#define DATASET_H

#include <GellyDataTypes.h>

#include "FrameParser.h"
#include "RTFRLoader.h"

namespace rtfr {
class Dataset {
private:
	std::vector<byte *> frameMemory;
	std::vector<FrameHeader> frameHeaders;

	void LoadFrame(uint frameIndex, const std::filesystem::path &framePath);
	void LoadAllFrameFiles(const DatasetInfo &info);

public:
	Dataset() = default;  // Vectors are going to be empty, so its fine.
	explicit Dataset(const DatasetInfo &info);
	~Dataset();

	void LoadDataset(const DatasetInfo &info);

	[[nodiscard]] bool IsLoaded() const;
	/**
	 * \brief Retrieves the position of a particle from a given frame.
	 * \param particleIndex Index of the particle to get.
	 * \param frameIndex Index of the frame to get the particle from.
	 * \return Pointer to a float array containing the position of the particle.
	 * Usually this is three floats.
	 */
	[[nodiscard]] float *GetParticle(uint particleIndex, uint frameIndex) const;
	[[nodiscard]] uint GetParticleCount(uint frameIndex) const;
};
}  // namespace rtfr

#endif	// DATASET_H
