#pragma once

#include <memory>
#include <vector>

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

	uint32_t AcquireNextImage(VkFence iFence, uint32_t iFrameIndex);

	void SubmitAsync(VulkanCommandBuffer* iCmd, VkFence iFence, VkSemaphore iWaitSemaphore = nullptr);
	void SubmitSync(VulkanCommandBuffer* iCmd, VulkanFence* iFence);

	// A Semaphore representing the last command executed on GPU Before presenting
	void Present(uint32_t iImageIndex, VkSemaphore iSemaphore);

	uint32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }
	uint32_t GetCurrentInFlightFrame() const { return mCurrentFrameInFlight; }

	void Flush();

	VkQueue mQueue = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;
	VulkanSwapchain* mSwapchain = nullptr;

	std::vector<std::unique_ptr<VulkanSemaphore>> mPresentSemaphore;

private:
	uint32_t mCurrentImageIndex = 0;
	uint32_t mCurrentFrameInFlight = 0;
};