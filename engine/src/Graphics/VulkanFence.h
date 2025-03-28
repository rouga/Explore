#pragma once

#include <vulkan/vulkan.h>

class VulkanFence 
{
public:
	VulkanFence(VkDevice iDevice, VkFenceCreateFlags iFlags);
	~VulkanFence();

	void Reset();
	void Wait();

	VkFence mFence;
	VkDevice mDevice;
};