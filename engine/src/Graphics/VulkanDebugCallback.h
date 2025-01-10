#pragma once

#include <vulkan/vulkan.h>

class VulkanInstance;

class VulkanDebugCallback
{
public:
	VulkanDebugCallback();
	~VulkanDebugCallback();

	void Initialize(VulkanInstance* iInstance);

private:
	VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
	VkInstance mInstance = nullptr;
};