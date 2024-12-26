#pragma once

#include <vulkan/vulkan.h>

class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();

	void Initialize(const char* iAppName);

	VkInstance GetInstance() const { return mInstance; }

private:
	VkInstance mInstance = VK_NULL_HANDLE;
};