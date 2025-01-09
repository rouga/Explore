#pragma once

#include <memory>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Transformation
{
public:
	void SetPosition(const glm::vec3& iPos) { mPosition = iPos; mIsTransformationDirty = true; }
	void SetRotation(const glm::vec3& iAxis, float iAngle);
	void SetScale(const glm::vec3& iScale) { mScale = iScale; mIsTransformationDirty = true; }

	glm::vec3 GetPosition() const { return mPosition; }
	glm::quat GetRotation() const { return mRotation; }
	glm::vec3 GetScale() const { return mScale; }

	const glm::mat4& GetMatrix();

	void SetParent(const std::shared_ptr<Transformation>& iParentTransform) { mParent = iParentTransform; }
	std::shared_ptr<Transformation> GetParent() const { return mParent; }

private:
	void UpdateTransformation();

	std::shared_ptr<Transformation> mParent;

	glm::vec3 mPosition{ 0 };    // Position of the model
	glm::quat mRotation{ 1.0f, 0.0f, 0.0f, 0.0f };    // Rotation using quaternion
	glm::vec3 mScale{ 1 };       // Scale of the model
	glm::mat4 mModelMatrix{ 1 }; // Final transformation matrix

	bool mIsTransformationDirty = true;
};