#include "fluidsim/CD3D11CPUSimData.h"

CD3D11CPUSimData::CD3D11CPUSimData()
	: positionBuffer(nullptr), velocityBuffer(nullptr) {}

void CD3D11CPUSimData::LinkBuffer(SimBufferType type, void *buffer) {
	switch (type) {
		case SimBufferType::POSITION:
			positionBuffer = static_cast<ID3D11Buffer *>(buffer);
			break;
		case SimBufferType::VELOCITY:
			velocityBuffer = static_cast<ID3D11Buffer *>(buffer);
			break;
	}
}

bool CD3D11CPUSimData::IsBufferLinked(SimBufferType type) {
	switch (type) {
		case SimBufferType::POSITION:
			return positionBuffer != nullptr;
		case SimBufferType::VELOCITY:
			return velocityBuffer != nullptr;
	}
	return false;
}

void *CD3D11CPUSimData::GetLinkedBuffer(SimBufferType type) {
	switch (type) {
		case SimBufferType::POSITION:
			return positionBuffer;
		case SimBufferType::VELOCITY:
			return velocityBuffer;
	}
	return nullptr;
}

SimContextAPI CD3D11CPUSimData::GetAPI() { return SimContextAPI::D3D11; }