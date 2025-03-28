#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

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
#include "DescriptorSetManager.h"
#include "PipelineLayoutManager.h"
#include "VulkanImage.h"

class Window;

class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	void Initialize(Window* iWindow);

	void Resize(int iWidth, int iHeight);

	void CopyImage(VkCommandBuffer iCmd, VkImage iSrcImage,
		VkImage iDstImage,
		uint32_t iWidth,
		uint32_t iHeight,
		uint32_t iMipLevel = 0,
		uint32_t iLayerCount = 1);

	void BlitImage(VkCommandBuffer iCmd, VkImage iSrcImage, VkImage iDstImage, VkOffset3D iSrcOffset, VkOffset3D iDstOffset);
	uint32_t GetNumSwapchainBuffers() { return mSwapchain->GetNumImages(); }
	uint32_t GetNumFramesInFlight() { return GetNumSwapchainBuffers() - 1; }

	std::unique_ptr<VulkanInstance> mInstance = nullptr;
	std::unique_ptr<VulkanDebugCallback> mDebugCallback = nullptr;
	std::unique_ptr<VulkanPhysicalDevice> mPhysicalDevice = nullptr;
	std::shared_ptr<VulkanLogicalDevice> mLogicalDevice = nullptr;
	VmaAllocator mAllocator = VK_NULL_HANDLE;
	std::unique_ptr<VulkanSwapchain> mSwapchain = nullptr;
	std::unique_ptr<VulkanQueue> mQueue = nullptr;
	std::unique_ptr<DescriptorSetManager> mDescriptorSetManager = nullptr;
	std::unique_ptr<PipelineLayoutManager> mPipelineLayoutManager = nullptr;
	std::vector<std::unique_ptr<VulkanFence>> mCompleteFences;
	std::unique_ptr<VulkanFence> mCopyFence = nullptr;

	VkCommandPool mCmdPool = VK_NULL_HANDLE;
	std::vector<VulkanCommandBuffer> mCmds;
	std::vector<VulkanCommandBuffer> mUICmds;
	VulkanCommandBuffer mCopyCmd;

	Window* mWindow = nullptr;

	std::unique_ptr<VulkanGPUBuffer> mStagingBuffer = nullptr;

private:
	void CreateAllocator();
	void CreateCommandBuffers();
	void CreateStagingBuffer();
};
