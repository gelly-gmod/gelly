#ifndef SCENE_H
#define SCENE_H
#include <memory>

#include "handlers/shape-handler.h"
#include "object-handler.h"

class Scene {
public:
	explicit Scene(ObjectHandlerContext ctx);
	~Scene() = default;

	[[nodiscard]] std::shared_ptr<ShapeHandler> GetShapeHandler() const;
	void Update();

private:
	using HandlerPtr = std::shared_ptr<ObjectHandler>;

	ObjectHandlerContext ctx;
	std::shared_ptr<MonotonicCounter> sharedCounter;

	std::shared_ptr<ShapeHandler> shapeHandler;
	std::vector<HandlerPtr> handlers;
};

#endif	// SCENE_H
