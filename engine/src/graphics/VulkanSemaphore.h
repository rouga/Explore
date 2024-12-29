#pragma once

#include "vulkan/vulkan.h"

class VulkanSemaphore
{
public:
	VulkanSemaphore(VkDevice iDevice);
	~VulkanSemaphore();

	VkSemaphore mSemaphore = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;
};