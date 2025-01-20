#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Entity.h"

class Viewport;

enum class CameraMode
{
	eFreeFly,
	eOrbit,
};

class OrbitCamera
{
	friend class Camera;
public:
	OrbitCamera(float iDistance = 10.0f, float iPitch = 0.0f, float iYaw = 0.0f, const glm::vec3& iTarget = glm::vec3(0.0f));

	void Update(float iDeltaTime);
	void UpdatePosition();
	float mOrbitDistance;
	glm::vec3 mPosition{ 0.f, 0.f, 0.f };
	float mFov = 60.f;
	float mNear = 0.1f;
	float mFar = 10000.f;

private:

	void setTarget(const glm::vec3& target);
	void setDistance(float distance);
	void setPitch(float pitch);
	void setYaw(float yaw);

	void adjustDistance(float delta);
	void adjustPitch(float delta);
	void adjustYaw(float delta);

	float mPitch = 0; // Angle in degrees
	float mYaw = 0;   // Angle in degrees
	glm::vec3 mTarget{ 0.f, 0.f, 0.f };
};

class FreeFlyCamera
{
	friend class Camera;
public:
	FreeFlyCamera();

	void Update(float iDeltaTime);

	float mSpeed = 10.0f; 
	float mSensitivity = 0.1f; 
	float mFov = 60.f;
	float mNear = 0.1f;
	float mFar = 10000.f;
	glm::vec3 mPosition{ 0.f, 0.f, 0.f };
private:
	void UpdateVectors();
	glm::vec3 mTarget{ 0.f, 0.f, 0.f };
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp{ 0.f, 1.0f, 0.f };
	float mPitch = 0; // Angle in degrees
	float mYaw = 90;   // Angle in degrees
};

class Camera : public Entity
{
public:
	Camera(Viewport* iWindow);

	void Update(float iDeltaTime);
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

	void SetMode(CameraMode iMode) {mMode = iMode; }
	CameraMode GetMode() const {return mMode; }

	OrbitCamera* GetOrbit() const { return mOrbitCamera.get(); }
	FreeFlyCamera* GetFreeFly() const { return mFreeFlyCamera.get(); }

private:
	CameraMode mMode = CameraMode::eFreeFly;
	Viewport* mViewport = nullptr;
	std::unique_ptr<OrbitCamera> mOrbitCamera = nullptr;
	std::unique_ptr<FreeFlyCamera> mFreeFlyCamera = nullptr;
};