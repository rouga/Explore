#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Window;

class OrbitCamera
{
public:
	OrbitCamera(Window* iWindow, float iDistance = 6.0f, float iPitch = 0.0f, float iYaw = 0.0f, const glm::vec3& iTarget = glm::vec3(0.0f));

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

private:
	Window* mWindow;
	glm::vec3 mTarget;
	float mDistance;
	float mPitch; // Angle in degrees
	float mYaw;   // Angle in degrees
};