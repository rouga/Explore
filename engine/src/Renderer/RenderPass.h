#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "Graphics/VulkanRenderPass.h"
#include "Graphics/VulkanGraphicsPipeline.h"

class RenderContext;
struct FrameResources;

class RenderPass
{
public:
	RenderPass(RenderContext* iContext);

	virtual void Setup(VkCommandBuffer iCmd) = 0;
	virtual void Begin(VkCommandBuffer iCmd) = 0;
	virtual void Draw(VkCommandBuffer iCmd, FrameResources* iFrameResources) = 0;
	virtual void End(VkCommandBuffer iCmd) = 0;

protected:

	std::unique_ptr<VulkanRenderPass> mRenderPass = nullptr;
	std::unique_ptr<VulkanGraphicsPipeline> mPipeline = nullptr;
	RenderContext* mContext = nullptr;
};