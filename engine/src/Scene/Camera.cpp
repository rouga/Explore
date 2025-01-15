#include "Camera.h"

#include <iostream>

#include "Renderer/Viewport.h"
#include "Core/Input.h"

OrbitCamera::OrbitCamera(Viewport* iViewport, float iDistance, float iPitch, float iYaw, const glm::vec3& iTarget)
	:mViewport(iViewport),
	mDistance(iDistance),
	mPitch(iPitch),
	mYaw(iYaw),
	mTarget(iTarget)
{
}

void OrbitCamera::Update()
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

void OrbitCamera::setTarget(const glm::vec3& iTarget) {
	mTarget = iTarget;
}

void OrbitCamera::setDistance(float iDistance) {
	mDistance = glm::clamp(iDistance, 1.0f, iDistance); // Prevent negative or extreme zoom
}

void OrbitCamera::setPitch(float iPitch) {
	mPitch = glm::clamp(iPitch, -89.0f, 89.0f); // Prevent flipping
}

void OrbitCamera::setYaw(float iYaw) {
	mYaw = iYaw;
}

void OrbitCamera::adjustDistance(float iDelta) {
	setDistance(mDistance + iDelta);
}

void OrbitCamera::adjustPitch(float iDelta) {
	setPitch(mPitch + iDelta);
}

void OrbitCamera::adjustYaw(float iDelta) {
	setYaw(mYaw + iDelta);
}

glm::mat4 OrbitCamera::getViewMatrix() const {
	// Calculate camera position
	glm::vec3 position = getPosition();

	// Look at the target
	return glm::lookAt(position, mTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 OrbitCamera::GetProjectionMatrix() const
{
	return glm::perspectiveFov(glm::radians(45.f),
		static_cast<float>(mViewport->GetWidth()),
		static_cast<float>(mViewport->GetHeight()),
		0.1f, 10000.f);
}

glm::vec3 OrbitCamera::getPosition() const {
	// Convert spherical coordinates to Cartesian
	float wPitchRad = glm::radians(mPitch);
	float wYawRad = glm::radians(mYaw);

	float wX = mDistance * cos(wPitchRad) * sin(wYawRad);
	float wY = mDistance * sin(wPitchRad);
	float wZ = mDistance * cos(wPitchRad) * cos(wYawRad);

	return mTarget + glm::vec3(wX, wY, wZ);
}
