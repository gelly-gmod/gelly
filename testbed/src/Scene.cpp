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

static std::vector<WorldRenderObject> renderObjects;
static SceneMetadata currentSceneMetadata;

void testbed::LoadScene(const SceneMetadata &metadata) {
	currentSceneMetadata = metadata;
	// Load glTF file at metadata.filepath

	fastgltf::Parser parser;
	fastgltf::GltfDataBuffer fileData;
	std::filesystem::path path = metadata.filepath;
	fileData.loadFromFile(path);

	auto asset = parser.loadGLTF(
		&fileData, path.parent_path(), fastgltf::Options::LoadGLBBuffers
	);

	if (auto err = asset.error(); err != fastgltf::Error::None) {
		GetLogger()->Error("Failed to load glTF scene");
		return;
	}

	auto gltfScene = asset->scenes[0];
	auto sceneNodes = gltfScene.nodeIndices;
	auto nodes = asset->nodes;

	GetLogger()->Info("Loaded glTF scene");

	for (const auto &gltfNode : nodes) {
		GetLogger()->Info("Loading node");
		WorldRenderObject renderObject = {};
		auto transform = gltfNode.transform;

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

		auto trs = std::get_if<fastgltf::Node::TRS>(&transform);
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
			auto transformArray =
				std::get<fastgltf::Node::TransformMatrix>(transform);

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					// Collapse into 1D coordinates
					transformMatrix.m[i][j] = transformArray[i * 4 + j];
				}
			}
		}

		renderObject.transform = transformMatrix;

		WorldMesh mesh = {};
		auto meshIndex = gltfNode.meshIndex;
		if (!meshIndex.has_value()) {
			// Can be used for really awesome stuff, like special markers,
			// but we dont have any of that yet
			continue;
		}

		auto gltfMesh = asset->meshes[meshIndex.value()];
		auto gltfPrimitives = gltfMesh.primitives;

		// This really only matters if we have multiple materials
		// per mesh, which we don't, so we can just grab the first
		// primitive

		auto gltfPrimitive = gltfPrimitives[0];

		auto gltfPositions = gltfPrimitive.findAttribute("POSITION");
		auto gltfNormals = gltfPrimitive.findAttribute("NORMAL");

		auto positions = asset->accessors[gltfPositions->second];
		auto normals = asset->accessors[gltfNormals->second];
		auto indices = asset->accessors[gltfPrimitive.indicesAccessor.value()];

		// allocate space for the vertices
		mesh.vertexCount = positions.count;
		mesh.vertices = new float3[mesh.vertexCount];

		// allocate space for the normals
		mesh.normalCount = normals.count;
		mesh.normals = new float3[mesh.normalCount];

		// allocate space for the indices
		mesh.indexCount = indices.count;
		mesh.indices = new unsigned short[mesh.indexCount];

		// memcpy from the buffers

		auto positionOffset = positions.byteOffset;
		auto normalOffset = normals.byteOffset;
		auto indexOffset = indices.byteOffset;

		auto &posBufferView =
			asset->bufferViews[positions.bufferViewIndex.value()];

		auto &normBufferView =
			asset->bufferViews[normals.bufferViewIndex.value()];

		auto &indexBufferView =
			asset->bufferViews[indices.bufferViewIndex.value()];

		auto posTrueOffset = posBufferView.byteOffset + positionOffset;
		auto normTrueOffset = normBufferView.byteOffset + normalOffset;
		auto indexTrueOffset = indexBufferView.byteOffset + indexOffset;

		auto posBuffer = asset->buffers[posBufferView.bufferIndex];
		auto normBuffer = asset->buffers[normBufferView.bufferIndex];
		auto indexBuffer = asset->buffers[indexBufferView.bufferIndex];

		auto &posBytes =
			std::get<fastgltf::sources::Vector>(posBuffer.data).bytes;

		auto &normBytes =
			std::get<fastgltf::sources::Vector>(normBuffer.data).bytes;

		auto &indexBytes =
			std::get<fastgltf::sources::Vector>(indexBuffer.data).bytes;

		memcpy(
			mesh.vertices,
			posBytes.data() + posTrueOffset,
			positions.count * sizeof(float3)
		);

		memcpy(
			mesh.normals,
			normBytes.data() + normTrueOffset,
			normals.count * sizeof(float3)
		);

		memcpy(
			mesh.indices,
			indexBytes.data() + indexTrueOffset,
			indices.count * sizeof(unsigned short)
		);

		GetLogger()->Info("Loaded mesh");

		auto meshReference = CreateWorldMesh(mesh);
		renderObject.mesh = meshReference;
		renderObjects.push_back(renderObject);

		GetLogger()->Info("Created render object");

		delete[] mesh.vertices;
		delete[] mesh.normals;
		delete[] mesh.indices;
	}
}

static float y = 0.0f;
void testbed::RenderScene() {
	// Assemble a render list
	WorldRenderList renderList = {};
	renderList.objectCount = renderObjects.size();
	renderList.objects = renderObjects.data();

	RenderWorldList(renderList, GetCamera());
}

SceneMetadata testbed::GetCurrentSceneMetadata() {
	return currentSceneMetadata;
}