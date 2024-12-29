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
	std::unique_ptr<VulkanLogicalDevice> mLogicalDevice = nullptr;
	std::unique_ptr<VulkanSwapchain> mSwapchain = nullptr;
	std::unique_ptr<VulkanQueue> mQueue = nullptr;

	VkCommandPool mCmdPool = VK_NULL_HANDLE;
	std::vector<VulkanCommandBuffer> mCmds;

private:
	void CreateCommandBuffers();
	void RecordCommandBuffers();
};
