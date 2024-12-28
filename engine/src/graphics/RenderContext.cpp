#include "RenderContext.h"

#include "Window.h"

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

}
