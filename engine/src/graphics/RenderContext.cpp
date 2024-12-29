#include "RenderContext.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Window.h"
#include "Utils.h"

RenderContext::RenderContext()
{
	mInstance = std::make_unique<VulkanInstance>();
#if defined(_DEBUG)
	mDebugCallback = std::make_unique<VulkanDebugCallback>();
#endif
	mPhysicalDevice = std::make_unique<VulkanPhysicalDevice>();
	mLogicalDevice = std::make_unique<VulkanLogicalDevice>();
	mSwapchain = std::make_unique<VulkanSwapchain>();
	mQueue = std::make_unique<VulkanQueue>();
}

RenderContext::~RenderContext()
{
	mQueue->Flush();
	vkDestroyCommandPool(mLogicalDevice->mDevice, mCmdPool, nullptr);
}

void RenderContext::Initialize(Window* iWindow)
{
	mInstance->Initialize("Explore Editor");
#if defined(_DEBUG)
	mDebugCallback->Initialize(mInstance.get());
#endif
	mPhysicalDevice->Initialize(mInstance->GetInstance());
	mLogicalDevice->Initialize(mPhysicalDevice.get());
	mSwapchain->Initialize(mInstance->GetInstance(), mLogicalDevice.get(), iWindow, 2);
	mQueue->Initialize(mLogicalDevice->mDevice, mSwapchain->mSwapchain, mLogicalDevice->mPhysicalDevice->GetQueueFamilyIndex(), 0);\
	CreateCommandBuffers();
	RecordCommandBuffers();
	mQueue->Flush();
}

void RenderContext::CreateCommandBuffers()
{
	// Create Command Pool
	VkCommandPoolCreateInfo wCmdPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = (uint32_t)mPhysicalDevice->GetQueueFamilyIndex()
	};

	VkResult wResult = vkCreateCommandPool(mLogicalDevice->mDevice, &wCmdPoolCreateInfo, nullptr, &mCmdPool);
	CHECK_VK_RESULT(wResult, "Command Pool Creation");

	// Allocate Command Buffers
	mCmds.resize(mSwapchain->GetNumImages());

	for(uint32_t i = 0; i < mSwapchain->GetNumImages(); i++)
	{
		mCmds[i] = VulkanCommandBuffer{mCmdPool, mLogicalDevice->mDevice};
	}

	spdlog::info("Command Pool Created with {:d} Command buffers.", mSwapchain->GetNumImages());
}

void RenderContext::RecordCommandBuffers()
{
	VkClearColorValue wClearColor = {1.f, 0.0f, 0.0f, 1.f};

	VkImageSubresourceRange wSubresourceRange =
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	for (uint32_t i = 0; i < mCmds.size(); i++)
	{
		VkImageMemoryBarrier wPresentToClear = 
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = mSwapchain->mImages[i],
			.subresourceRange = wSubresourceRange
		};

		VkImageMemoryBarrier wToPresent =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = mSwapchain->mImages[i],
			.subresourceRange = wSubresourceRange
		};

		mCmds[i].Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		vkCmdPipelineBarrier(mCmds[i].mCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &wPresentToClear);

		vkCmdClearColorImage(mCmds[i].mCmd, mSwapchain->mImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &wClearColor, 1, &wSubresourceRange);

		vkCmdPipelineBarrier(mCmds[i].mCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &wToPresent);

		mCmds[i].End();
	}

	spdlog::info("Command Buffers Recorded.");
}
