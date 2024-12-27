#pragma once 

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;

class VulkanLogicalDevice
{
public:
	VulkanLogicalDevice();
	~VulkanLogicalDevice();

	void Initialize(VulkanPhysicalDevice* iPhysicalDevice);

	VkDevice mDevice = VK_NULL_HANDLE;
};