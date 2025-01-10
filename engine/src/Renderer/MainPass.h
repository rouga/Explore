#pragma once

#include "RenderPass.h"
#include "graphics/VulkanShader.h"

class VulkanGPUBuffer;

class MainPass : public RenderPass
{
public:
	MainPass(RenderContext* iContext);

	virtual void Setup(VkCommandBuffer iCmd, FrameResources* iFrameResources) override;
	virtual void Begin(VkCommandBuffer iCmd, FrameResources* iFrameResources) override;
	virtual void Draw(VkCommandBuffer iCmd, FrameResources* iFrameResources) override;
	virtual void End(VkCommandBuffer iCmd, FrameResources* iFrameResources) override;

	std::unique_ptr<VulkanShader> mVS = nullptr;
	std::unique_ptr<VulkanShader> mFS = nullptr;
};