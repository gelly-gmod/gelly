#include "Scene.h"

#include <DirectXMath.h>

#include <fastgltf/parser.hpp>
#include <vector>

#include "Camera.h"
#include "Logging.h"
#include "Rendering.h"
#include "Window.h"
#include "fastgltf/tools.hpp"

// Since everything is static, we really don't need to care
// much about the game-side of things. We just store the render objects
// once parsed

using namespace testbed;

/**
 * \brief Contains the mesh of a collision object in the scene and its render
 * object. The render object can be accessed to figure out the position and
 * orientation of the object.
 */
struct SceneCollisionObject {
	WorldRenderObject *renderObject;
	ObjectHandle sceneHandle;
	MeshDataVector vertices;
	MeshDataVector indices;
};

static ILogger *logger = nullptr;

static std::vector<WorldRenderObject> renderObjects;
static std::vector<SceneCollisionObject> collisionObjects;
static SceneMetadata currentSceneMetadata;

void testbed::InitializeSceneSystem(ILogger *newLogger) { logger = newLogger; }

void DestroyOldScene() {
	for (const auto &renderObject : renderObjects) {
		DestroyWorldMesh(renderObject.mesh);
	}
	renderObjects.clear();
}

void testbed::LoadScene(const SceneMetadata &metadata) {
	if (!renderObjects.empty()) {
		DestroyOldScene();
	}

	currentSceneMetadata = metadata;
	// Load glTF file at metadata.filepath

	fastgltf::Parser parser;
	fastgltf::GltfDataBuffer fileData;
	std::filesystem::path path = metadata.filepath;
	fileData.loadFromFile(path);

	auto asset = parser.loadGLTF(
		&fileData,
		path.parent_path(),
		fastgltf::Options::LoadGLBBuffers |
			fastgltf::Options::LoadExternalBuffers
	);

	if (auto err = asset.error(); err != fastgltf::Error::None) {
		logger->Error("Failed to load glTF scene: '%s'", metadata.filepath);
		return;
	}

	const auto &nodes = asset->nodes;

	logger->Info("Loaded glTF scene: '%s'", metadata.filepath);

	for (const auto &gltfNode : nodes) {
		logger->Info("Loading node");
		const auto &transform = gltfNode.transform;

		// Convert from fastgltf 4x4 to DirectX 4x4
		//		auto transform = gltfNode.transform;
		//		auto transformArray =
		//			std::get<fastgltf::Node::TransformMatrix>(transform);
		//
		//		XMFLOAT4X4 transformMatrix = {};
		//		for (int i = 0; i < 4; i++) {
		//			for (int j = 0; j < 4; j++) {
		//				// Collapse into 1D coordinates
		//				transformMatrix.m[i][j] = transformArray[i * 4 + j];
		//			}
		//		}

		XMFLOAT4X4 transformMatrix = {};

		const auto &trs = std::get_if<fastgltf::Node::TRS>(&transform);
		if (trs) {
			XMVECTOR translation = XMVectorSet(
				trs->translation[0],
				trs->translation[1],
				trs->translation[2],
				1.0f
			);
			XMVECTOR rotation = XMVectorSet(
				trs->rotation[0],
				trs->rotation[1],
				trs->rotation[2],
				trs->rotation[3]
			);
			XMVECTOR scale =
				XMVectorSet(trs->scale[0], trs->scale[1], trs->scale[2], 1.0f);

			XMStoreFloat4x4(
				&transformMatrix,
				XMMatrixAffineTransformation(
					scale, XMVectorZero(), rotation, translation
				)
			);
		} else {
			// Has to be one or the other
			const auto &transformArray =
				std::get<fastgltf::Node::TransformMatrix>(transform);

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					// Collapse into 1D coordinates
					transformMatrix.m[i][j] = transformArray[i * 4 + j];
				}
			}
		}

		const auto &meshIndex = gltfNode.meshIndex;
		if (!meshIndex.has_value()) {
			// Can be used for really awesome stuff, like special markers,
			// but we dont have any of that yet
			continue;
		}

		const auto &gltfMesh = asset->meshes[meshIndex.value()];
		const auto &gltfPrimitives = gltfMesh.primitives;

		// This really only matters if we have multiple materials
		// per mesh, which we don't, so we can just grab the first
		// primitive

		for (const auto &gltfPrimitive : gltfPrimitives) {
			WorldMesh mesh = {};
			const auto &gltfPositions = gltfPrimitive.findAttribute("POSITION");
			const auto &gltfNormals = gltfPrimitive.findAttribute("NORMAL");

			const auto &positions = asset->accessors[gltfPositions->second];
			const auto &normals = asset->accessors[gltfNormals->second];
			const auto &indices =
				asset->accessors[gltfPrimitive.indicesAccessor.value()];

			const auto &positionOffset = positions.byteOffset;
			const auto &normalOffset = normals.byteOffset;
			const auto &indexOffset = indices.byteOffset;

			const auto &posBufferView =
				asset->bufferViews[positions.bufferViewIndex.value()];

			const auto &normBufferView =
				asset->bufferViews[normals.bufferViewIndex.value()];

			const auto &indexBufferView =
				asset->bufferViews[indices.bufferViewIndex.value()];

			const auto &posBuffer = asset->buffers[posBufferView.bufferIndex];
			const auto &normBuffer = asset->buffers[normBufferView.bufferIndex];
			const auto &indexBuffer =
				asset->buffers[indexBufferView.bufferIndex];

			auto &posBytes =
				std::get<fastgltf::sources::Vector>(posBuffer.data).bytes;

			auto &normBytes =
				std::get<fastgltf::sources::Vector>(normBuffer.data).bytes;

			auto &indexBytes =
				std::get<fastgltf::sources::Vector>(indexBuffer.data).bytes;

			// We can't just copy-assign the buffer as it requires an offset
			auto posTrueOffset = posBufferView.byteOffset + positionOffset;
			auto normTrueOffset = normBufferView.byteOffset + normalOffset;
			auto indexTrueOffset = indexBufferView.byteOffset + indexOffset;

			mesh.vertices.resize(positions.count * sizeof(float3));
			mesh.normals.resize(normals.count * sizeof(float3));
			mesh.indices.resize(indices.count * sizeof(unsigned short));

			std::memcpy(
				mesh.vertices.data(),
				posBytes.data() + posTrueOffset,
				mesh.vertices.size()
			);

			std::memcpy(
				mesh.normals.data(),
				normBytes.data() + normTrueOffset,
				mesh.normals.size()
			);

			std::memcpy(
				mesh.indices.data(),
				indexBytes.data() + indexTrueOffset,
				mesh.indices.size()
			);

			logger->Info(
				"Loaded mesh, vertices: %d, normals: %d, indices: %d",
				mesh.vertices.size() / sizeof(float3),
				mesh.normals.size() / sizeof(float3),
				mesh.indices.size() / sizeof(unsigned short)
			);

			// Update scene metadata
			currentSceneMetadata.triangles += mesh.indices.size() / 3;

			auto meshReference = CreateWorldMesh(mesh);

			WorldRenderObject renderObject = {};
			renderObject.transform = transformMatrix;
			renderObject.mesh = meshReference;
			renderObjects.push_back(renderObject);

			// Create collision object
			SceneCollisionObject collisionObject = {};
			collisionObject.renderObject = &renderObjects.back();
			// The renderer just reads the vertices and indices from the
			// mesh, it doesn't own them, so we can just move them.
			// The normals will be dropped, but we don't need them for
			// collision detection
			collisionObject.sceneHandle = INVALID_OBJECT_HANDLE;
			collisionObject.vertices = std::move(mesh.vertices);
			collisionObject.indices = std::move(mesh.indices);
			collisionObjects.push_back(collisionObject);
		}

		logger->Info("Created render object");
	}
}

void testbed::RenderScene() { RenderWorldList(renderObjects, GetCamera()); }

SceneMetadata testbed::GetCurrentSceneMetadata() {
	return currentSceneMetadata;
}

void testbed::RegisterSceneToGellySim(IFluidSimulation *sim) {
	auto *scene = sim->GetScene();
	for (auto &collisionObject : collisionObjects) {
		ObjectCreationParams params = {};
		params.shape = ObjectShape::TRIANGLE_MESH;
		auto shapeData = ObjectCreationParams::TriangleMesh{};

		shapeData.vertices =
			reinterpret_cast<float *>(collisionObject.vertices.data());
		shapeData.indices =
			reinterpret_cast<uint *>(collisionObject.indices.data());

		shapeData.vertexCount =
			collisionObject.vertices.size() / sizeof(float3);
		shapeData.indexCount =
			collisionObject.indices.size() / sizeof(unsigned short);

		// Decompose transform matrix
		auto &transform = collisionObject.renderObject->transform;
		const auto loadedTransform = XMLoadFloat4x4(&transform);
		const XMVECTOR translation = XMVectorSet(
			transform._41, transform._42, transform._43, transform._44
		);
		const XMVECTOR rotation = XMQuaternionRotationMatrix(loadedTransform);
		const XMVECTOR scale = XMVectorSet(
			XMVectorGetX(loadedTransform.r[0]),
			XMVectorGetY(loadedTransform.r[1]),
			XMVectorGetZ(loadedTransform.r[2]),
			1.0f
		);

		// In the creation stage we set the scale, but since we have static
		// transforms, we'll also set the position and scale here

		shapeData.scale[0] = XMVectorGetX(scale);
		shapeData.scale[1] = XMVectorGetY(scale);
		shapeData.scale[2] = XMVectorGetZ(scale);

		collisionObject.sceneHandle = scene->CreateObject(params);

		scene->SetObjectPosition(
			collisionObject.sceneHandle,
			XMVectorGetX(translation),
			XMVectorGetY(translation),
			XMVectorGetZ(translation)
		);

		scene->SetObjectQuaternion(
			collisionObject.sceneHandle,
			XMVectorGetX(rotation),
			XMVectorGetY(rotation),
			XMVectorGetZ(rotation),
			XMVectorGetW(rotation)
		);
	}
}

void testbed::UnregisterSceneFromGellySim(IFluidSimulation *sim) {
	auto *scene = sim->GetScene();
	for (auto &collisionObject : collisionObjects) {
		scene->RemoveObject(collisionObject.sceneHandle);
		collisionObject.sceneHandle = INVALID_OBJECT_HANDLE;
	}
}

void testbed::UpdateGellySimScene() {
	// Nothing for now, but in the future this could be used to make
	// cool stuff like a wave pool
}
