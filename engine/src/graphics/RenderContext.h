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
#include "VulkanGraphicsPipeline.h"
#include "VulkanGPUBuffer.h"

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
	VulkanCommandBuffer mCopyCmd;

	Window* mWindow = nullptr;

	std::unique_ptr<VulkanShader> mVS = nullptr;
	std::unique_ptr<VulkanShader> mFS = nullptr;
	std::unique_ptr<VulkanGraphicsPipeline> mPipeline = nullptr;

	std::unique_ptr<VulkanGPUBuffer> mStagingBuffer = nullptr;

private:
	void CreateCommandBuffers();
	void RecordCommandBuffers();

	void CreateStagingBuffer();
};
