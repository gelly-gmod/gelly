#include "scene.h"

using namespace gelly::simulation;

Scene::Scene(ObjectHandlerContext ctx) : ctx(ctx) {
	sharedCounter = std::make_shared<MonotonicCounter>();
	shapeHandler = std::make_shared<ShapeHandler>(ctx, sharedCounter);
	handlers.push_back(shapeHandler);
}

Scene::Scene() : ctx({}) {}

std::shared_ptr<ShapeHandler> Scene::GetShapeHandler() const {
	return shapeHandler;
}

void Scene::Update() {
	for (const auto &handler : handlers) {
		handler->Update();
	}
}