#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Entity.h"

class Viewport;

class Camera : public Entity
{
public:
	Camera(Viewport* iWindow, float iDistance = 6.0f, float iPitch = 0.0f, float iYaw = 0.0f, const glm::vec3& iTarget = glm::vec3(0.0f));

	void Update();

	void setTarget(const glm::vec3& target);
	void setDistance(float distance);
	void setPitch(float pitch);
	void setYaw(float yaw);

	void adjustDistance(float delta);
	void adjustPitch(float delta);
	void adjustYaw(float delta);

	glm::mat4 getViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	glm::vec3 getPosition() const;

	float mDistance;
	float mFov = 60.f;
	float mNear = 0.1f;
	float mFar = 10000.f;
private:
	Viewport* mViewport;
	glm::vec3 mTarget;
	float mPitch; // Angle in degrees
	float mYaw;   // Angle in degrees
};