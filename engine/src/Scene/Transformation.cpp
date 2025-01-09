#include "Transformation.h"

void Transformation::SetRotation(const glm::vec3& iAxis, float iAngle)
{
	glm::vec3 wNormalizedAxis = glm::normalize(iAxis);

	// Create a quaternion from the axis and angle
	mRotation = glm::angleAxis(glm::radians(iAngle), iAxis);

	mIsTransformationDirty = true;
}

const glm::mat4& Transformation::GetMatrix()
{
	if (mIsTransformationDirty)
	{
		UpdateTransformation();
	}

	return mModelMatrix;
}

void Transformation::UpdateTransformation()
{
	glm::mat4 wTranslation = glm::translate(glm::mat4(1.0f), mPosition);
	glm::mat4 wRotationMatrix = glm::toMat4(mRotation); // Convert quaternion to matrix
	glm::mat4 wScaling = glm::scale(glm::mat4(1.0f), mScale);

	// Combine transformations: T * R * S
	mModelMatrix = wTranslation * wRotationMatrix * wScaling;
	if(mParent)
	{
		mModelMatrix = mParent->GetMatrix() * mModelMatrix;
	}

	mIsTransformationDirty = false;
}
