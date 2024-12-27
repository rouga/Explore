#pragma once

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice
{
public:
	VulkanPhysicalDevice();
	~VulkanPhysicalDevice();

	void Initialize(VkInstance iInstance);

	VkPhysicalDevice GetDevice() const { return mDevice; }

private:
	void LogSelectedDevice();

	VkPhysicalDevice mDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties mDeviceProperties;
};