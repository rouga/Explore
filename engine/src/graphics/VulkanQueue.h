#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "VulkanSemaphore.h"

class VulkanCommandBuffer;
class VulkanSemaphore;
class VulkanSwapchain;
class VulkanFence;

class VulkanQueue
{
public:
	VulkanQueue();
	~VulkanQueue();

	void Initialize(VkDevice iDevice, VulkanSwapchain* iSwapchain, uint32_t iQueueFamily, uint32_t iQueueIndex);

	uint32_t AcquireNextImage();

	void SubmitAsync(VulkanCommandBuffer* iCmd, VkFence iFence);
	void SubmitSync(VulkanCommandBuffer* iCmd, VulkanFence* iFence);

	// A Semaphore representing the last command executed on GPU Before presenting
	void Present(uint32_t iImageIndex, VkSemaphore iSemaphore);

	uint32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }

	void Flush();

	VkQueue mQueue = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;
	VulkanSwapchain* mSwapchain = nullptr;

	std::unique_ptr<VulkanSemaphore> mPresentSemaphore = nullptr;

private:
	uint32_t mCurrentImageIndex = 0;
};