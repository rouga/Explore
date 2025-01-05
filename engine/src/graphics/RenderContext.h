#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "VulkanInstance.h"
#include "VulkanDebugCallback.h"
#include "VulkanSwapchain.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"
#include "VulkanShader.h"
#include "VulkanGPUBuffer.h"
#include "VulkanFence.h"
#include "VulkanMemoryPool.h"
#include "vma/vk_mem_alloc.h"

class Window;

class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	void Initialize(Window* iWindow);

	std::unique_ptr<VulkanInstance> mInstance = nullptr;
	std::unique_ptr<VulkanDebugCallback> mDebugCallback = nullptr;
	std::unique_ptr<VulkanPhysicalDevice> mPhysicalDevice = nullptr;
	std::shared_ptr<VulkanLogicalDevice> mLogicalDevice = nullptr;
	VmaAllocator mAllocator = VK_NULL_HANDLE;
	std::unique_ptr<VulkanSwapchain> mSwapchain = nullptr;
	std::unique_ptr<VulkanQueue> mQueue = nullptr;
	std::vector<std::unique_ptr<VulkanFence>> mFences;

	VkCommandPool mCmdPool = VK_NULL_HANDLE;
	std::vector<VulkanCommandBuffer> mCmds;
	VulkanCommandBuffer mCopyCmd;

	Window* mWindow = nullptr;

	std::unique_ptr<VulkanGPUBuffer> mStagingBuffer = nullptr;

private:
	void CreateAllocator();
	void CreateCommandBuffers();
	void CreateStagingBuffer();
};
