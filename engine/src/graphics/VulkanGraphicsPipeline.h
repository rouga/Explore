#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class Window;

class VulkanGraphicsPipeline
{
public:
	VulkanGraphicsPipeline();
	~VulkanGraphicsPipeline();

	void Initialize(VkDevice iDevice, Window* iWindow, VkFormat iColorFormat,
		const std::vector<VkDescriptorSetLayout>& iDescriptorSetLayout,
		VkShaderModule iVS, VkShaderModule iFS);

	VkPipeline mPipeline = VK_NULL_HANDLE;
	VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;

private:

	VkDevice mDevice = VK_NULL_HANDLE;
};