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
	mLogicalDevice = std::make_shared<VulkanLogicalDevice>();
	mSwapchain = std::make_unique<VulkanSwapchain>();
	mQueue = std::make_unique<VulkanQueue>();
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

	mFences.resize(mSwapchain->GetNumImages());
	for (uint32_t i = 0; i < mSwapchain->GetNumImages(); i++)
	{
		mFences[i] = std::make_unique<VulkanFence>(mLogicalDevice->mDevice, VK_FENCE_CREATE_SIGNALED_BIT);
	}

	CreateCommandBuffers();
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
	mStagingBuffer->Initialize(mLogicalDevice.get(), 1024*1024*4);
}
