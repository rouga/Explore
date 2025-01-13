#pragma once

#include <vector>
#include <vulkan/vulkan.h>


class Window;

// Vulkan Render Pass class uses the dynamic rendering feature
class VulkanRenderPass
{
public:
	VulkanRenderPass(VkDevice iDevice);

	void Begin(VkCommandBuffer iCmd, std::vector<VkImageView> iColorImages, VkImageView iDepthImageView, VkExtent2D iExtent) const;
	void Begin(VkCommandBuffer iCmd, 
						 const std::vector<VkRenderingAttachmentInfo> iColorAttachmentsInfo,
						 const VkRenderingAttachmentInfo* iDepthAttachmentInfo,
						 VkExtent2D iExtent) const;
	void End(VkCommandBuffer iCmd) const;

private:
	VkDevice mDevice = VK_NULL_HANDLE;
};