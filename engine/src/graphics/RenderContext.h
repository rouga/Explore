#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "VulkanInstance.h"
#include "VulkanDebugCallback.h"
#include "VulkanSwapchain.h"
#include "VulkanPhysicalDevice.h"

class Window;

class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	void Initialize(Window* iWindow);

	std::unique_ptr<VulkanInstance> mInstance = nullptr;
	std::unique_ptr<VulkanDebugCallback> mDebugCallback = nullptr;
	std::unique_ptr<VulkanSwapchain> mSwapchain = nullptr;
	std::unique_ptr<VulkanPhysicalDevice> mPhysicalDevice = nullptr;
};
