#pragma once

#include "RenderPass.h"
#include "graphics/VulkanShader.h"

class MainPass : public RenderPass
{
public:
	MainPass(RenderContext* iContext);

	virtual void Setup(VkCommandBuffer iCmd) override;
	virtual void Begin(VkCommandBuffer iCmd) override;
	virtual void Draw(VkCommandBuffer iCmd) override;
	virtual void End(VkCommandBuffer iCmd) override;

	std::unique_ptr<VulkanShader> mVS = nullptr;
	std::unique_ptr<VulkanShader> mFS = nullptr;
};