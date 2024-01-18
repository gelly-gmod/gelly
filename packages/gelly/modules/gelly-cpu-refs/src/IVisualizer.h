#ifndef IVISUALIZER_H
#define IVISUALIZER_H

class __declspec(novtable) IVisualizer {
public:
	virtual ~IVisualizer() = default;

	/**
	 * \brief Called for each frame, during rendering
	 * \note This is the best place to draw anything
	 */
	virtual void OnNewFrame() = 0;
	/**
	 * \brief Called when the underlying algorithm for the visualizer should
	 * start
	 */
	virtual void Start() = 0;

	virtual const char *GetCurrentStatus() const = 0;
	virtual const char *GetName() const = 0;
};

#endif	// IVISUALIZER_H
