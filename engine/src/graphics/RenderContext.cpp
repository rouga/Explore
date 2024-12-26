#include "RenderContext.h"

RenderContext::RenderContext()
{
	mInstance = std::make_unique<VulkanInstance>();
	mDebugCallback = std::make_unique<VulkanDebugCallback>();
}

RenderContext::~RenderContext()
{
}

void RenderContext::Initialize()
{
	mInstance->Initialize("Explore Editor");
#if defined(_DEBUG)
	mDebugCallback->Initialize(mInstance.get());
#endif
}
