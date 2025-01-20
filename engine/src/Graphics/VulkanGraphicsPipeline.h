#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class Window;

class VulkanGraphicsPipeline
{
public:
	struct PipelineInfo {
		VkPipelineRenderingCreateInfo renderingInfo{};
		VkPipelineViewportStateCreateInfo viewportInfo{};
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		std::vector<VkDynamicState> dynamicStates;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		uint32_t subpass = 0;
	};

	VulkanGraphicsPipeline();
	~VulkanGraphicsPipeline();

	void Initialize(VkDevice iDevice, const PipelineInfo iPipelineInfo, const VkShaderModule iVS, VkShaderModule iFS);
	void Bind(VkCommandBuffer iCmd, Window* iWindow, bool iFlip = false);
	void Bind(VkCommandBuffer iCmd);

	static void DefaultPipelineConfigInfo(PipelineInfo& iConfigInfo);

	VkPipeline mPipeline = VK_NULL_HANDLE;

private:

	VkDevice mDevice = VK_NULL_HANDLE;
};