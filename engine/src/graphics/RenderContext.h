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
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanFence.h"

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
	std::unique_ptr<VulkanSwapchain> mSwapchain = nullptr;
	std::unique_ptr<VulkanQueue> mQueue = nullptr;
	std::vector<std::unique_ptr<VulkanFence>> mFences;

	VkCommandPool mCmdPool = VK_NULL_HANDLE;
	std::vector<VulkanCommandBuffer> mCmds;
	VulkanCommandBuffer mCopyCmd;

	std::unique_ptr<VulkanDescriptorPool> mDescriptorPool;
	std::vector<std::unique_ptr<VulkanDescriptorSet>> mDescriptorSets;

	Window* mWindow = nullptr;

	std::unique_ptr<VulkanShader> mVS = nullptr;
	std::unique_ptr<VulkanShader> mFS = nullptr;
	std::unique_ptr<VulkanGraphicsPipeline> mPipeline = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mStagingBuffer = nullptr;

private:
	void CreateDescriptorSets();
	
	void CreateCommandBuffers();

	void CreateStagingBuffer();
};
