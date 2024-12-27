#include "RenderContext.h"

#include "Window.h"

RenderContext::RenderContext()
{
	mInstance = std::make_unique<VulkanInstance>();
	mDebugCallback = std::make_unique<VulkanDebugCallback>();
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
	mSwapchain->Initialize(mInstance->GetInstance(), iWindow);
}
