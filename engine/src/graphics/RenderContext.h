#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "VulkanInstance.h"
#include "VulkanDebugCallback.h"

class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	void Initialize();
	std::unique_ptr<VulkanInstance> mInstance = nullptr;

	std::unique_ptr<VulkanDebugCallback> mDebugCallback = nullptr;
};
