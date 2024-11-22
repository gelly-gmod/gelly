#include "scene.h"

Scene::Scene(ObjectHandlerContext ctx) : ctx(ctx) {
	shapeHandler = std::make_shared<ShapeHandler>(ctx, sharedCounter);
	handlers.push_back(shapeHandler);
}

std::shared_ptr<ShapeHandler> Scene::GetShapeHandler() const {
	return shapeHandler;
}

void Scene::Update() {
	for (const auto &handler : handlers) {
		handler->Update();
	}
}