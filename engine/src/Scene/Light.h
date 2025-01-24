#pragma once

#include <glm/glm.hpp>

#include "Scene/Entity.h"

class Light : public Entity
{
public:
	enum class LightType
	{
		eDirectional,
		ePoint,
		eSpot,
		eMax
	};

	struct DirectionalLightConfig
	{
		float mIntensity = 1.0f;
		float mAzimuth = 0; // Degrees
		float mElevation = 60; // Degrees
		glm::vec3 mColor = {1.f, 1.f, 1.f};
	};
	
	Light(const DirectionalLightConfig& iConfig);

	void Update();

	LightType GetType() const { return mType; }
	glm::vec3 GetDirection() const { return mDirection; }
	
	float mIntensity{ 0 };
	float mAzimuth = 0;
	float mElevation = 0;
	glm::vec3 mPosition{ 0 };
	glm::vec3 mColor{ 0 };

private:
	// Function to compute direction vector from azimuth and elevation
	glm::vec3 ComputeDirection(float azimuth, float elevation);

	glm::vec3 mDirection{ 0 };
	LightType mType = LightType::eMax;

};