#pragma once

#include "RenderPass.h"

class VulkanGPUBuffer;
class VulkanDescriptorPool;

class UIPass : public RenderPass
{
public:
	UIPass(RenderContext* iContext);

	virtual void Setup(VkCommandBuffer iCmd, FrameResources* iFrameResources) override;
	virtual void Begin(VkCommandBuffer iCmd, FrameResources* iFrameResources) override;
	virtual void Draw(VkCommandBuffer iCmd, FrameResources* iFrameResources) override;
	virtual void End(VkCommandBuffer iCmd, FrameResources* iFrameResources) override;

	std::unique_ptr<VulkanDescriptorPool> mPool;
};