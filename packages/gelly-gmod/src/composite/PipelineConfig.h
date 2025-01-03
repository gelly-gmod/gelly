#ifndef PIPELINECONFIG_H
#define PIPELINECONFIG_H

/**
 * Configuration for the pipeline (e.g. gelly settings or other settings)
 */
struct PipelineConfig {
	float particleRadius;
	float thresholdRatio = 0.01f;

	float diffuseMotionBlur = 25.f;
	float diffuseScale = 0.23f;

	float cubemapStrength = 1.f;
	float refractionStrength = 0.03f;
	float whitewaterStrength = 1.f;

	float diffuseColor[3] = {0.8f, 0.8f, 0.8f};
	float sunDirection[3] = {0.5f, 0.5f, 0.5f};
	float sunEnabled = 0.f;
};

#endif	// PIPELINECONFIG_H
