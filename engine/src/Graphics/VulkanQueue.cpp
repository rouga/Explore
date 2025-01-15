#include "VulkanQueue.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"
#include "VulkanFence.h"

#include "Utils.h"

VulkanQueue::VulkanQueue()
{
}

VulkanQueue::~VulkanQueue()
{
}

void VulkanQueue::Initialize(VkDevice iDevice, VulkanSwapchain* iSwapchain, uint32_t iQueueFamily, uint32_t iQueueIndex)
{
	mDevice = iDevice;
	mSwapchain = iSwapchain;

	mPresentSemaphore = std::make_unique<VulkanSemaphore>(iDevice);
	
	vkGetDeviceQueue(iDevice, iQueueFamily, iQueueIndex, &mQueue);
	spdlog::info("Queue {0:d} Acquired from Family {1:d}", iQueueIndex, iQueueFamily);
}

uint32_t VulkanQueue::AcquireNextImage(VkFence iFence)
{
	VkResult wResult = vkAcquireNextImageKHR(mDevice, mSwapchain->mSwapchain, UINT64_MAX, mPresentSemaphore->mSemaphore, iFence, &mCurrentImageIndex);
	CHECK_VK_RESULT(wResult, "Acquire Next Image");

	return mCurrentImageIndex;
}

void VulkanQueue::SubmitAsync(VulkanCommandBuffer* iCmd, VkFence iFence)
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
		.pSignalSemaphores = &(iCmd->mCmdSubmitSemaphore->mSemaphore)
	};

	VkResult wResult = vkQueueSubmit(mQueue, 1, &wSubmitInfo, iFence);
	CHECK_VK_RESULT(wResult, "Command Queue Submit");
}

void VulkanQueue::SubmitSync(VulkanCommandBuffer* iCmd, VulkanFence* iFence)
{
	VkSubmitInfo wSubmitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = VK_NULL_HANDLE,
		.pWaitDstStageMask = VK_NULL_HANDLE,
		.commandBufferCount = 1,
		.pCommandBuffers = &iCmd->mCmd,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = VK_NULL_HANDLE
	};

	VkResult wResult = vkQueueSubmit(mQueue, 1, &wSubmitInfo, iFence->mFence);

	iFence->Wait();
	iFence->Reset();
	CHECK_VK_RESULT(wResult, "Command Queue Submit");
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
		.pSwapchains = &mSwapchain->mSwapchain,
		.pImageIndices = &iImageIndex
	};

	VkResult wResult = vkQueuePresentKHR(mQueue, &wPresentInfo);
	CHECK_VK_RESULT(wResult, "Queue Present");
}

void VulkanQueue::Flush()
{
	vkQueueWaitIdle(mQueue);
}