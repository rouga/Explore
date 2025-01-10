#pragma once

#include <string>

#include <vulkan/vulkan.h>

class VulkanShader
{
public:
	enum class EStage
	{
		Vertex,
		Fragment,
		Compute,
		Unknown
	};

	VulkanShader();
	~VulkanShader();

	void Initialize(VkDevice iDevice, const std::string& iFilePath);

	VkShaderModule mShader = VK_NULL_HANDLE;
	EStage mStage = EStage::Unknown;
	VkDevice mDevice = VK_NULL_HANDLE;

};