#include "RenderContext.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Window.h"
#include "Utils.h"

RenderContext::RenderContext()
{
	mInstance = std::make_unique<VulkanInstance>();
	mDebugCallback = std::make_unique<VulkanDebugCallback>();
	mPhysicalDevice = std::make_unique<VulkanPhysicalDevice>();
	mLogicalDevice = std::make_unique<VulkanLogicalDevice>();
	mSwapchain = std::make_unique<VulkanSwapchain>();
}

RenderContext::~RenderContext()
{
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
	CreateCommandBuffers();
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
	VkCommandBufferAllocateInfo wCmdAllocateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = mCmdPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = mSwapchain->GetNumImages(),
	};

	wResult = vkAllocateCommandBuffers(mLogicalDevice->mDevice, &wCmdAllocateInfo, mCmds.data());
	CHECK_VK_RESULT(wResult, "Command Buffer Allocation");

	spdlog::info("Command Pool Created with {:d} Command buffers.", mSwapchain->GetNumImages());
}
