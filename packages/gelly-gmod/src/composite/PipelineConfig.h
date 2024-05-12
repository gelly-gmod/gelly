#ifndef PIPELINECONFIG_H
#define PIPELINECONFIG_H

/**
 * Configuration for the pipeline (e.g. gelly settings or other settings)
 */
struct PipelineConfig {
	float particleRadius;
	float thresholdRatio;

	float filterIterations;
	float thicknessIterations;
};

#endif	// PIPELINECONFIG_H
