#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "../IVisualizer.h"

class CExampleVisualizer : public IVisualizer {
private:
	float m_hue = 0.0f;

public:
	CExampleVisualizer() = default;
	~CExampleVisualizer() override = default;

	void OnNewFrame() override;
	void Start() override;

	const char *GetCurrentStatus() const override;
	const char *GetName() const override;
};

#endif	// EXAMPLE_H
