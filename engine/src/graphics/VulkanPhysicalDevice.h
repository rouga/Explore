#pragma once

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice
{
public:
	VulkanPhysicalDevice();
	~VulkanPhysicalDevice();

	void Initialize(VkInstance iInstance);

	VkPhysicalDevice GetDevice() const { return mDevice; }
	int							 GetQueueFamilyIndex() const { return mQueueFamilyIndex; }
	const VkPhysicalDeviceMemoryProperties& GetMemProperties() const { return mMemProperties; }

private:
	void QueryDevice();
	void QueryQueueFamily();
	void LogSelectedDevice();

	VkInstance mInstance = VK_NULL_HANDLE;
	VkPhysicalDevice mDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties mDeviceProperties;
	VkPhysicalDeviceMemoryProperties mMemProperties;
	int mQueueFamilyIndex = -1;
};