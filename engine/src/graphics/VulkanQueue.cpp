#include "VulkanQueue.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "VulkanCommandBuffer.h"
#include "Utils.h"

VulkanQueue::VulkanQueue()
{
}

VulkanQueue::~VulkanQueue()
{
	vkDestroySemaphore(mDevice, mPresentSemaphore->mSemaphore, nullptr);
}

void VulkanQueue::Initialize(VkDevice iDevice, VkSwapchainKHR iSwapchain, uint32_t iQueueFamily, uint32_t iQueueIndex)
{
	mDevice = iDevice;
	mSwapchain = iSwapchain;

	mPresentSemaphore = std::make_unique<VulkanSemaphore>(iDevice);
	
	vkGetDeviceQueue(iDevice, iQueueFamily, iQueueIndex, &mQueue);
	spdlog::info("Queue {0:d} Acquired from Family {1:d}", iQueueIndex, iQueueFamily);
}

uint32_t VulkanQueue::AcquireNextImage()
{
	uint32_t wImageIndex = 0;
	VkResult wResult = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mPresentSemaphore->mSemaphore, nullptr, &wImageIndex);
	CHECK_VK_RESULT(wResult, "Acquire Next Image");

	return wImageIndex;
}

void VulkanQueue::SubmitAsync(VulkanCommandBuffer* iCmd)
{
	VkPipelineStageFlags wWaitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo wSubmitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &mPresentSemaphore->mSemaphore,
		.pWaitDstStageMask = &wWaitFlags,
		.commandBufferCount = 1,
		.pCommandBuffers = &iCmd->mCmd,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &(iCmd->mCmdFinishSemaphore->mSemaphore)
	};

	VkResult wResult = vkQueueSubmit(mQueue, 1, &wSubmitInfo, nullptr);
	CHECK_VK_RESULT(wResult, "Command Queue Submit");
}

void VulkanQueue::SubmitSync(VulkanCommandBuffer* iCmd)
{
}

void VulkanQueue::Present(uint32_t iImageIndex, VkSemaphore iSemaphore)
{
	VkPresentInfoKHR wPresentInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &iSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &mSwapchain,
		.pImageIndices = &iImageIndex
	};

	VkResult wResult = vkQueuePresentKHR(mQueue, &wPresentInfo);
	CHECK_VK_RESULT(wResult, "Queue Present");
}

void VulkanQueue::Flush()
{
	vkQueueWaitIdle(mQueue);
}