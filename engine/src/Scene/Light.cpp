#include "Light.h"

Light::Light(const DirectionalLightConfig& iConfig)
	:Entity(EntityType::eLight)
{
	mType = LightType::eDirectional;
	mIntensity = iConfig.mIntensity;
	mAzimuth = iConfig.mAzimuth;
	mElevation = iConfig.mElevation;
	mColor = iConfig.mColor;

	ComputeDirection(mAzimuth, mElevation);
}

void Light::Update()
{
	if(mType == LightType::eDirectional)
	{
		mDirection = ComputeDirection(mAzimuth, mElevation);
	}
}

glm::vec3 Light::ComputeDirection(float azimuth, float elevation)
{
	// Convert degrees to radians
	float wAzimuthRad = glm::radians(azimuth);
	float wElevationRad = glm::radians(elevation);

	// Calculate direction vector
	float x = glm::cos(wElevationRad) * glm::sin(wAzimuthRad); // East-West component
	float y = glm::sin(wElevationRad);                        // Vertical (Up-Down)
	float z = glm::cos(wElevationRad) * glm::cos(wAzimuthRad); // North-South component

	return glm::vec3(x, y, z);
}
