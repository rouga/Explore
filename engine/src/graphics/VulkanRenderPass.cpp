#include "VulkanRenderPass.h"

#include "Window.h"


VulkanRenderPass::VulkanRenderPass(VkDevice iDevice, VkExtent2D iScreenExtent)
	:mDevice(iDevice),
	mScreenExtent(iScreenExtent)
{}

void VulkanRenderPass::Begin(VkCommandBuffer iCmd, std::vector<VkImageView> iColorImages, VkImageView iDepthImageView) const
{
	std::vector<VkRenderingAttachmentInfo> wColorAttachments(iColorImages.size());

	// Setup color attachment
	for (uint32_t i = 0; i < iColorImages.size(); i++)
	{
		wColorAttachments[i] =
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = nullptr,
			.imageView = iColorImages[i],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearValue{.color = {{0.0f, 0.0f, 0.0f, 1.0f}} }
		};
	}

	// Setup depth attachment
	VkRenderingAttachmentInfo wDepthAttachment =
	{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = nullptr,
			.imageView = iDepthImageView,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = VkClearValue{.depthStencil = {1.0f, 0} }
	};

	// Setup render pass info
	VkRenderingInfo wRenderPassInfo =
	{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderArea = { {0, 0}, mScreenExtent },
			.layerCount = 1,
			.colorAttachmentCount = (uint32_t)wColorAttachments.size(),
			.pColorAttachments = wColorAttachments.data(),
			.pDepthAttachment = iDepthImageView ? &wDepthAttachment : nullptr,
	};

	vkCmdBeginRendering(iCmd, &wRenderPassInfo);
}

void VulkanRenderPass::End(VkCommandBuffer iCmd) const
{
	vkCmdEndRendering(iCmd);
}
