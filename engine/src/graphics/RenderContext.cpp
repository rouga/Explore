#include "RenderContext.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

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
	mQueue->Initialize(mLogicalDevice->mDevice, mSwapchain->mSwapchain, mLogicalDevice->mPhysicalDevice->GetQueueFamilyIndex(), 0);

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
	mStagingBuffer->Initialize(mLogicalDevice.get(), 128 * 1024 * 1024, mAllocator);
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
