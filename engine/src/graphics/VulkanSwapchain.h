#pragma once

#include <vulkan/vulkan.h>

class VulkanInstance;
class Window;

class VulkanSwapchain
{
public:
	VulkanSwapchain(){}
	~VulkanSwapchain();
	
	void Initialize(VkInstance iInstance, Window* iWindow);

private:
	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	VkInstance mInstance = VK_NULL_HANDLE;
};