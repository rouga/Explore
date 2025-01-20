#include "Camera.h"

#include <iostream>

#include "Renderer/Viewport.h"
#include "Core/Input.h"
#include "Entity.h"

OrbitCamera::OrbitCamera(float iDistance, float iPitch, float iYaw, const glm::vec3& iTarget)
	:mOrbitDistance(iDistance)
{
	mPitch = iPitch;
	mYaw = iYaw;
	mTarget = iTarget;
	UpdatePosition();
}

void OrbitCamera::Update(float iDeltaTime)
{
	if(!Input::Get().IsCursorOnUI())
	{
		if (Input::Get().IsKeyDown(GLFW_KEY_R))
		{
			setPitch(20.f);
			setYaw(45.0f);
			setDistance(10.0f);
		}
		else if (Input::Get().IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			auto wDelta = Input::Get().GetMouseDelta();

			if (wDelta.first != 0 || wDelta.second != 0)
			{
				adjustYaw(static_cast<float>(wDelta.first * 0.4f));
				adjustPitch(static_cast<float>(-wDelta.second * 0.4f));
			}
		}
		adjustDistance(Input::Get().GetScrollOffset().second);
		UpdatePosition();
	}
}

void OrbitCamera::setTarget(const glm::vec3& iTarget) {
	mTarget = iTarget;
}

void OrbitCamera::setDistance(float iDistance) {
	mOrbitDistance = glm::clamp(iDistance, 1.0f, iDistance); // Prevent negative or extreme zoom
}

void OrbitCamera::setPitch(float iPitch) {
	mPitch = glm::clamp(iPitch, -89.0f, 89.0f); // Prevent flipping
}

void OrbitCamera::setYaw(float iYaw) {
	mYaw = iYaw;
}

void OrbitCamera::adjustDistance(float iDelta) {
	setDistance(mOrbitDistance + iDelta);
}

void OrbitCamera::adjustPitch(float iDelta) {
	setPitch(mPitch + iDelta);
}

void OrbitCamera::adjustYaw(float iDelta) {
	setYaw(mYaw + iDelta);
}

void OrbitCamera::UpdatePosition() {
	// Convert spherical coordinates to Cartesian
	float wPitchRad = glm::radians(mPitch);
	float wYawRad = glm::radians(mYaw);

	float wX = mOrbitDistance * cos(wPitchRad) * sin(wYawRad);
	float wY = mOrbitDistance * sin(wPitchRad);
	float wZ = mOrbitDistance * cos(wPitchRad) * cos(wYawRad);

	mPosition = mTarget + glm::vec3(wX, wY, wZ);
}

FreeFlyCamera::FreeFlyCamera()
{
	UpdateVectors();
}

void FreeFlyCamera::Update(float iDeltaTime)
{
	if (!Input::Get().IsCursorOnUI())
	{
		float wVelocity = mSpeed * iDeltaTime;
		if (Input::Get().IsKeyPressed(GLFW_KEY_W)) mPosition += mFront * wVelocity;   // Move forward
		if (Input::Get().IsKeyPressed(GLFW_KEY_S)) mPosition -= mFront * wVelocity;   // Move backward
		if (Input::Get().IsKeyPressed(GLFW_KEY_A)) mPosition -= mRight * wVelocity;   // Move left
		if (Input::Get().IsKeyPressed(GLFW_KEY_D)) mPosition += mRight * wVelocity;   // Move right
		if (Input::Get().IsKeyPressed(GLFW_KEY_E)) mPosition += mWorldUp * wVelocity; // Move up
		if (Input::Get().IsKeyPressed(GLFW_KEY_Q)) mPosition -= mWorldUp * wVelocity; // Move down

		mFov = glm::clamp(mFov + Input::Get().GetScrollOffset().second, 1.0, 100.0);

		if(Input::Get().IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			auto wDelta = Input::Get().GetMouseDelta();
			if (wDelta.first != 0 || wDelta.second != 0)
			{
				mYaw = mYaw + mSensitivity * wDelta.first;
				mPitch = glm::clamp(mPitch + mSensitivity * wDelta.second, -89.0, 89.0);
				UpdateVectors();
			}
		}
		mTarget = mPosition + mFront;
	};
}

void FreeFlyCamera::UpdateVectors()
{
	glm::vec3 wNewFront;
	wNewFront.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	wNewFront.y = sin(glm::radians(mPitch));
	wNewFront.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront = glm::normalize(wNewFront);

	mRight = glm::normalize(glm::cross(mFront, mWorldUp));
	mUp = glm::normalize(glm::cross(mRight, mFront));
}

Camera::Camera(Viewport* iWindow)
	:Entity(EntityType::eCamera),
	mViewport(iWindow)
{
	mOrbitCamera = std::make_unique<OrbitCamera>();
	mFreeFlyCamera = std::make_unique<FreeFlyCamera>();
}

void Camera::Update(float iDeltaTime)
{
	mMode == CameraMode::eOrbit ? mOrbitCamera->Update(iDeltaTime) : mFreeFlyCamera->Update(iDeltaTime);
}

glm::mat4 Camera::GetViewMatrix() const
{
	if(mMode == CameraMode::eOrbit)
	{
		return glm::lookAt(mOrbitCamera->mPosition, mOrbitCamera->mTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		return glm::lookAt(mFreeFlyCamera->mPosition, mFreeFlyCamera->mTarget, mFreeFlyCamera->mUp);
	}

}

glm::mat4 Camera::GetProjectionMatrix() const
{
	glm::mat4 wProjMatrix;
	if (mMode == CameraMode::eOrbit)
	{
		wProjMatrix = glm::perspectiveFov(glm::radians(mOrbitCamera->mFov),
			static_cast<float>(mViewport->GetWidth()),
			static_cast<float>(mViewport->GetHeight()),
			mOrbitCamera->mNear, mOrbitCamera->mFar);
	}
	else
	{
		wProjMatrix = glm::perspectiveFov(glm::radians(mFreeFlyCamera->mFov),
			static_cast<float>(mViewport->GetWidth()),
			static_cast<float>(mViewport->GetHeight()),
			mFreeFlyCamera->mNear, mFreeFlyCamera->mFar);
	}
	return wProjMatrix;
}
