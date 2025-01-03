#pragma once

#include <vector>
#include <vulkan/vulkan.h>


class Window;

// Vulkan Render Pass class uses the dynamic rendering feature
class VulkanRenderPass
{
public:
	VulkanRenderPass(VkDevice iDevice, VkExtent2D iScreenExtent);

	void Begin(VkCommandBuffer iCmd, std::vector<VkImageView> iColorImages, VkImageView iDepthImageView) const;
	void End(VkCommandBuffer iCmd) const;

private:
	VkDevice mDevice = VK_NULL_HANDLE;
	VkExtent2D mScreenExtent{0, 0};
};