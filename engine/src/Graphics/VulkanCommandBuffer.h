#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "VulkanSemaphore.h"

class VulkanCommandBuffer
{
public:
	VulkanCommandBuffer(){}
	VulkanCommandBuffer(VkCommandPool iCmdPool, VkDevice iDevice);

	void Begin(VkCommandBufferUsageFlags iFlags);
	void End();
	void Reset(VkCommandBufferResetFlags iFlags);

	VkCommandBuffer mCmd = VK_NULL_HANDLE;
	std::unique_ptr<VulkanSemaphore> mCmdSubmitSemaphore = nullptr;
};