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
	mVS = std::make_unique<VulkanShader>();
	mFS = std::make_unique<VulkanShader>();
	mPipeline = std::make_unique<VulkanGraphicsPipeline>();
	mStagingBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

RenderContext::~RenderContext()
{
	mQueue->Flush();
	vkDestroyCommandPool(mLogicalDevice->mDevice, mCmdPool, nullptr);
}

void RenderContext::Initialize(Window* iWindow)
{
	mWindow = iWindow;

	mInstance->Initialize("Explore Editor");
#if defined(_DEBUG)
	mDebugCallback->Initialize(mInstance.get());
#endif
	mPhysicalDevice->Initialize(mInstance->GetInstance());
	mLogicalDevice->Initialize(mPhysicalDevice.get());
	mSwapchain->Initialize(mInstance->GetInstance(), mLogicalDevice.get(), iWindow, 2);
	mQueue->Initialize(mLogicalDevice->mDevice, mSwapchain->mSwapchain, mLogicalDevice->mPhysicalDevice->GetQueueFamilyIndex(), 0);
	mVS->Initialize(mLogicalDevice->mDevice, "shaders/bin/basic.vert.spv");
	mFS->Initialize(mLogicalDevice->mDevice, "shaders/bin/basic.frag.spv");
	mPipeline->Initialize(mLogicalDevice->mDevice, iWindow, mSwapchain->mColorFormat, mVS->mShader, mFS->mShader);
	CreateCommandBuffers();
	RecordCommandBuffers();
	CreateStagingBuffer();
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

	mCopyCmd = VulkanCommandBuffer{ mCmdPool, mLogicalDevice->mDevice };

	spdlog::info("Command Pool Created with {:d} Command buffers.", mSwapchain->GetNumImages() + 1);
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

	VkViewport wViewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)mWindow->GetWidth(),
		.height = (float)mWindow->GetHeight(),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D wScissor =
	{
		.offset =
		{
			.x = 0,
			.y = 0
		},
		.extent =
		{
			.width = (uint32_t)mWindow->GetWidth(),
			.height = (uint32_t)mWindow->GetHeight()
		}
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

		const VkRenderingAttachmentInfo wColorAttachmentInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = mSwapchain->mImageViews[i],
			.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = wClearColor,
		};

		VkRect2D wRenderArea = 
		{
			.offset = { 0, 0},
			.extent = { (uint32_t)mWindow->GetWidth(), (uint32_t)mWindow->GetHeight() }
		};

		const VkRenderingInfo render_info
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = wRenderArea,
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &wColorAttachmentInfo
		};

		vkCmdBeginRendering(mCmds[i].mCmd, &render_info);

		vkCmdBindPipeline(mCmds[i].mCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->mPipeline);
		vkCmdSetViewport(mCmds[i].mCmd, 0, 1, &wViewport);
		vkCmdSetScissor(mCmds[i].mCmd, 0, 1, &wScissor);

		vkCmdDraw(mCmds[i].mCmd, 3, 1, 0, 0);

		vkCmdEndRendering(mCmds[i].mCmd);

		vkCmdPipelineBarrier(mCmds[i].mCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &wToPresent);

		mCmds[i].End();
	}

	spdlog::info("Command Buffers Recorded.");
}

void RenderContext::CreateStagingBuffer()
{
	mStagingBuffer->Initialize(mLogicalDevice.get(), 1024*1024*4);
}
