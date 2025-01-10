#include "RenderContext.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Core/Window.h"
#include "Utils.h"

RenderContext::RenderContext()
{
	mInstance = std::make_unique<VulkanInstance>();
#if defined(_DEBUG)
	mDebugCallback = std::make_unique<VulkanDebugCallback>();
#endif
	mPhysicalDevice = std::make_unique<VulkanPhysicalDevice>();
	mLogicalDevice = std::make_shared<VulkanLogicalDevice>();
	mSwapchain = std::make_unique<VulkanSwapchain>();
	mQueue = std::make_unique<VulkanQueue>();
}

RenderContext::~RenderContext()
{
	mQueue->Flush();
	vkDestroyCommandPool(mLogicalDevice->mDevice, mCmdPool, nullptr);
	mStagingBuffer->FreeGPU();
	mDepthBuffer->FreeGPU();
	vmaDestroyAllocator(mAllocator);
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

	CreateAllocator();

	mSwapchain->Initialize(mInstance->GetInstance(), mLogicalDevice.get(), iWindow, 2);
	CreateDepthBuffer();
	mQueue->Initialize(mLogicalDevice->mDevice, mSwapchain.get(), mLogicalDevice->mPhysicalDevice->GetQueueFamilyIndex(), 0);

	mStagingBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

	mDescriptorSetManager = std::make_unique<DescriptorSetManager>(mLogicalDevice->mDevice, mSwapchain->GetNumImages());
	mPipelineLayoutManager = std::make_unique<PipelineLayoutManager>(mLogicalDevice->mDevice);

	mCompleteFences.resize(mSwapchain->GetNumImages());
	for (uint32_t i = 0; i < mSwapchain->GetNumImages(); i++)
	{
		mCompleteFences[i] = std::make_unique<VulkanFence>(mLogicalDevice->mDevice, VK_FENCE_CREATE_SIGNALED_BIT);
	}

	mCopyFence = std::make_unique<VulkanFence>(mLogicalDevice->mDevice, 0);

	CreateCommandBuffers();
	CreateStagingBuffer();
	TransitionDepthBuffer();
	mQueue->Flush();
}

void RenderContext::Resize(int iWidth, int iHeight)
{
	mDepthBuffer->Resize(VkExtent3D{(uint32_t)iWidth, (uint32_t)iHeight, 1});
	mSwapchain->Resize(VkExtent2D{ (uint32_t)iWidth, (uint32_t)iHeight });
}

void RenderContext::CopyImage(VkCommandBuffer iCmd, VkImage iSrcImage, VkImage iDstImage,
															uint32_t iWidth, uint32_t iHeight, uint32_t iMipLevel, uint32_t iLayerCount)
{
	// Define the image copy region
	VkImageCopy wCopyRegion{};
	wCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	wCopyRegion.srcSubresource.mipLevel = iMipLevel;
	wCopyRegion.srcSubresource.baseArrayLayer = 0;
	wCopyRegion.srcSubresource.layerCount = iLayerCount;
	wCopyRegion.srcOffset = { 0, 0, 0 };

	wCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	wCopyRegion.dstSubresource.mipLevel = iMipLevel;
	wCopyRegion.dstSubresource.baseArrayLayer = 0;
	wCopyRegion.dstSubresource.layerCount = iLayerCount;
	wCopyRegion.dstOffset = { 0, 0, 0 };

	wCopyRegion.extent.width = iWidth;
	wCopyRegion.extent.height = iHeight;
	wCopyRegion.extent.depth = 1;

	// Record the image copy command
	vkCmdCopyImage(
		iCmd,
		iSrcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		iDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, // regionCount
		&wCopyRegion
	);
}

void RenderContext::BlitImage(VkCommandBuffer iCmd, VkImage iSrcImage, VkImage iDstImage, VkOffset3D iSrcOffset, VkOffset3D iDstOffset)
{
	VkImageBlit blitRegion{};
	blitRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	blitRegion.srcOffsets[0] = { 0, 0, 0 };
	blitRegion.srcOffsets[1] = iSrcOffset;
	blitRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	blitRegion.dstOffsets[0] = { 0, 0, 0 };
	blitRegion.dstOffsets[1] = iDstOffset;

	vkCmdBlitImage(iCmd, iSrcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		iDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion,
		VK_FILTER_LINEAR);
}

void RenderContext::CreateAllocator()
{
	// initialize the memory allocator
	VmaAllocatorCreateInfo wAllocatorInfo = {};
	wAllocatorInfo.physicalDevice = mPhysicalDevice->GetDevice();
	wAllocatorInfo.device = mLogicalDevice->mDevice;
	wAllocatorInfo.instance = mInstance->GetInstance();
	wAllocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	VkResult wResult = vmaCreateAllocator(&wAllocatorInfo, &mAllocator);

	CHECK_VK_RESULT(wResult, "Allocator Creation");

	spdlog::info("VMA Allocator Created");
}

void RenderContext::CreateDepthBuffer()
{
	VulkanImage::ImageConfig wImageConfig{};
	wImageConfig.format = VK_FORMAT_D24_UNORM_S8_UINT;
	wImageConfig.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	wImageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	mDepthBuffer = std::make_unique<VulkanImage>(mLogicalDevice->mDevice, mAllocator);
	mDepthBuffer->Initialize(VkExtent3D{
		.width = (uint32_t)mWindow->GetWidth(),
		.height =(uint32_t)mWindow->GetHeight(),
		.depth = 1
		}, wImageConfig);
}

void RenderContext::CreateCommandBuffers()
{
	// Create Command Pool
	VkCommandPoolCreateInfo wCmdPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
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

void RenderContext::CreateStagingBuffer()
{
	mStagingBuffer->Initialize(mLogicalDevice.get(), 64 * 1024 * 1024, mAllocator);
}

void RenderContext::TransitionDepthBuffer()
{
	mCopyCmd.Reset(0);
	mCopyCmd.Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	mDepthBuffer->Transition(mCopyCmd.mCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	mCopyCmd.End();

	mQueue->SubmitSync(&mCopyCmd, mCopyFence->mFence);
	mCopyFence->Wait();
	mCopyFence->Reset();

}
