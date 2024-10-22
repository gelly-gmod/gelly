#include "fluidsim/CD3D11CPUSimData.h"

CD3D11CPUSimData::CD3D11CPUSimData() :
	positionBuffer(nullptr), velocity0Buffer(nullptr), maxParticles(0) {}

void CD3D11CPUSimData::LinkBuffer(SimBufferType type, void *buffer) {
	switch (type) {
		case SimBufferType::POSITION:
			positionBuffer = static_cast<ID3D11Buffer *>(buffer);
			break;
		case SimBufferType::VELOCITY0:
			velocity0Buffer = static_cast<ID3D11Buffer *>(buffer);
			break;
		case SimBufferType::VELOCITY1:
			velocity1Buffer = static_cast<ID3D11Buffer *>(buffer);
			break;
		case SimBufferType::FOAM_POSITION:
			foamPositionBuffer = static_cast<ID3D11Buffer *>(buffer);
			break;
		case SimBufferType::FOAM_VELOCITY:
			foamVelocityBuffer = static_cast<ID3D11Buffer *>(buffer);
			break;
		case SimBufferType::ANISOTROPY_Q1:
			anisotropyQ1Buffer = static_cast<ID3D11Buffer *>(buffer);
			break;
		case SimBufferType::ANISOTROPY_Q2:
			anisotropyQ2Buffer = static_cast<ID3D11Buffer *>(buffer);
			break;
		case SimBufferType::ANISOTROPY_Q3:
			anisotropyQ3Buffer = static_cast<ID3D11Buffer *>(buffer);
			break;
	}
}

bool CD3D11CPUSimData::IsBufferLinked(SimBufferType type) {
	switch (type) {
		case SimBufferType::POSITION:
			return positionBuffer != nullptr;
		case SimBufferType::VELOCITY0:
			return velocity0Buffer != nullptr;
		case SimBufferType::VELOCITY1:
			return velocity1Buffer != nullptr;
		case SimBufferType::FOAM_POSITION:
			return foamPositionBuffer != nullptr;
		case SimBufferType::FOAM_VELOCITY:
			return foamVelocityBuffer != nullptr;
		case SimBufferType::ANISOTROPY_Q1:
			return anisotropyQ1Buffer != nullptr;
		case SimBufferType::ANISOTROPY_Q2:
			return anisotropyQ2Buffer != nullptr;
		case SimBufferType::ANISOTROPY_Q3:
			return anisotropyQ3Buffer != nullptr;
	}
	return false;
}

void *CD3D11CPUSimData::GetLinkedBuffer(SimBufferType type) {
	switch (type) {
		case SimBufferType::POSITION:
			return positionBuffer;
		case SimBufferType::VELOCITY0:
			return velocity0Buffer;
		case SimBufferType::VELOCITY1:
			return velocity1Buffer;
		case SimBufferType::FOAM_POSITION:
			return foamPositionBuffer;
		case SimBufferType::FOAM_VELOCITY:
			return foamVelocityBuffer;
		case SimBufferType::ANISOTROPY_Q1:
			return anisotropyQ1Buffer;
		case SimBufferType::ANISOTROPY_Q2:
			return anisotropyQ2Buffer;
		case SimBufferType::ANISOTROPY_Q3:
			return anisotropyQ3Buffer;
	}
	return nullptr;
}

SimContextAPI CD3D11CPUSimData::GetAPI() { return SimContextAPI::D3D11; }

void CD3D11CPUSimData::SetMaxParticles(const int maxParticles) {
	this->maxParticles = maxParticles;
}

int CD3D11CPUSimData::GetMaxParticles() { return maxParticles; }

void CD3D11CPUSimData::SetActiveParticles(const int activeParticles) {
	this->activeParticles = activeParticles;
}

int CD3D11CPUSimData::GetActiveParticles() { return activeParticles; }

void CD3D11CPUSimData::SetMaxFoamParticles(const int maxFoamParticles) {
	this->maxFoamParticles = maxFoamParticles;
}

int CD3D11CPUSimData::GetMaxFoamParticles() { return maxFoamParticles; }

void CD3D11CPUSimData::SetActiveFoamParticles(const int activeFoamParticles) {
	this->activeFoamParticles = activeFoamParticles;
}

int CD3D11CPUSimData::GetActiveFoamParticles() { return activeFoamParticles; }