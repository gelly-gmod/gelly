#ifndef PIPELINECONFIG_H
#define PIPELINECONFIG_H

/**
 * Configuration for the pipeline (e.g. gelly settings or other settings)
 */
struct PipelineConfig {
	float particleRadius;
	float thresholdRatio = 0.07f;

	float diffuseMotionBlur = 25.f;
	float diffuseScale = 0.23f;

	float filterIterations = 2.f;
	float thicknessIterations = 13.f;

	float cubemapStrength = 1.f;
};

#endif	// PIPELINECONFIG_H
