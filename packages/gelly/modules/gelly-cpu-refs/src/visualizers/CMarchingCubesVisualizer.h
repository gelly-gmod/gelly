#ifndef CMARCHINGCUBESVISUALIZER_H
#define CMARCHINGCUBESVISUALIZER_H

#define MARCHING_CUBES_IMPLEMENTATION
#include <gelly-cpu-refs/algo/marching-cubes.h>

#include "../CRaylibVisualDebugFacility.h"
#include "../IVisualizer.h"

class CMarchingCubesVisualizer : public IVisualizer {
private:
	vector<XMFLOAT4> m_points;
	CRaylibVisualDebugFacility m_visualDebugFacility;
	Camera3D m_camera;

public:
	CMarchingCubesVisualizer();
	~CMarchingCubesVisualizer() override = default;

	void OnNewFrame() override;
	void Start() override;
	const char *GetCurrentStatus() const override;
	const char *GetName() const override;
};

#endif	// CMARCHINGCUBESVISUALIZER_H
