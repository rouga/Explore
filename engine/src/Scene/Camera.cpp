#include "Camera.h"

#include <iostream>

#include "Renderer/Viewport.h"
#include "Core/Input.h"
#include "Entity.h"

Camera::Camera(Viewport* iViewport, float iDistance, float iPitch, float iYaw, const glm::vec3& iTarget)
	:Entity(EntityType::eCamera),
	mViewport(iViewport),
	mDistance(iDistance),
	mPitch(iPitch),
	mYaw(iYaw),
	mTarget(iTarget)
{
}

void Camera::Update()
{
	if (Input::Get().IsKeyDown(GLFW_KEY_R))
	{
		setPitch(20.f);
		setYaw(45.0f);
		setDistance(10.0f);
	}
	else if (!Input::Get().IsCursorOnUI() && Input::Get().IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
	{
		auto wDelta = Input::Get().GetMouseDelta();

		if (wDelta.first == 0 && wDelta.second == 0)
		{
			return;
		}
		else
		{
			adjustYaw(static_cast<float>(wDelta.first * 0.4f));
			adjustPitch(static_cast<float>(-wDelta.second * 0.4f));
		}
	}

	adjustDistance(Input::Get().GetScrollOffset().second);
}

void Camera::setTarget(const glm::vec3& iTarget) {
	mTarget = iTarget;
}

void Camera::setDistance(float iDistance) {
	mDistance = glm::clamp(iDistance, 1.0f, iDistance); // Prevent negative or extreme zoom
}

void Camera::setPitch(float iPitch) {
	mPitch = glm::clamp(iPitch, -89.0f, 89.0f); // Prevent flipping
}

void Camera::setYaw(float iYaw) {
	mYaw = iYaw;
}

void Camera::adjustDistance(float iDelta) {
	setDistance(mDistance + iDelta);
}

void Camera::adjustPitch(float iDelta) {
	setPitch(mPitch + iDelta);
}

void Camera::adjustYaw(float iDelta) {
	setYaw(mYaw + iDelta);
}

glm::mat4 Camera::getViewMatrix() const {
	// Calculate camera position
	glm::vec3 position = getPosition();

	// Look at the target
	return glm::lookAt(position, mTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	return glm::perspectiveFov(glm::radians(mFov),
		static_cast<float>(mViewport->GetWidth()),
		static_cast<float>(mViewport->GetHeight()),
		mNear, mFar);
}

glm::vec3 Camera::getPosition() const {
	// Convert spherical coordinates to Cartesian
	float wPitchRad = glm::radians(mPitch);
	float wYawRad = glm::radians(mYaw);

	float wX = mDistance * cos(wPitchRad) * sin(wYawRad);
	float wY = mDistance * sin(wPitchRad);
	float wZ = mDistance * cos(wPitchRad) * cos(wYawRad);

	return mTarget + glm::vec3(wX, wY, wZ);
}
