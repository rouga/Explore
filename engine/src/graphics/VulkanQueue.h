#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "VulkanSemaphore.h"

class VulkanCommandBuffer;
class VulkanSemaphore;

class VulkanQueue
{
public:
	VulkanQueue();
	~VulkanQueue();

	void Initialize(VkDevice iDevice, VkSwapchainKHR iSwapchain, uint32_t iQueueFamily, uint32_t iQueueIndex);

	uint32_t AcquireNextImage();

	void SubmitAsync(VulkanCommandBuffer* iCmd);
	void SubmitSync(VulkanCommandBuffer* iCmd);

	// A Semaphore representing the last command executed on GPU Before presenting
	void Present(uint32_t iImageIndex, VkSemaphore iSemaphore);

	void Flush();

	VkQueue mQueue = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;
	VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;

	std::unique_ptr<VulkanSemaphore> mPresentSemaphore = nullptr;
};