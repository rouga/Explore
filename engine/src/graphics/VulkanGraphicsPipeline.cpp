#include "VulkanGraphicsPipeline.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Window.h"
#include "Utils.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline()
{

}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
	vkDestroyPipeline(mDevice, mPipeline, nullptr);
}

void VulkanGraphicsPipeline::Initialize(VkDevice iDevice, Window* iWindow, VkFormat iColorFormat,
	const std::vector<VkDescriptorSetLayout>& iDescriptorSetLayout,
	const  VkShaderModule iVS, VkShaderModule iFS)
{
	mDevice = iDevice;

	VkPipelineShaderStageCreateInfo wShaderStageCreateInfo[2] =
	{
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = iVS,
			.pName = "main"
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = iFS,
			.pName = "main"
		}
	};

	VkPipelineVertexInputStateCreateInfo wVertexInputState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	};

	VkPipelineInputAssemblyStateCreateInfo wInputAssemblyState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	VkViewport wViewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)iWindow->GetWidth(),
		.height = (float)iWindow->GetHeight(),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D wScissor =
	{
		.offset = 
		{
			.x = 0,
			.y = 0
		},
		.extent =
		{
			.width = (uint32_t)iWindow->GetWidth(),
			.height = (uint32_t)iWindow->GetHeight()
		}
	};

	VkPipelineViewportStateCreateInfo wViewportState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &wViewport,
		.scissorCount = 1,
		.pScissors = &wScissor,
	};

	VkPipelineRasterizationStateCreateInfo wRasterizationState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.lineWidth = 1.0f
	};

	VkPipelineMultisampleStateCreateInfo wMultisampleState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f
	};

	VkPipelineColorBlendAttachmentState wColorBlendAttachment =
	{
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo wColorBlendState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &wColorBlendAttachment
	};

	VkPipelineLayoutCreateInfo wPipelineLayoutInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = (uint32_t)iDescriptorSetLayout.size(),
		.pSetLayouts = iDescriptorSetLayout.data()
	};

	VkResult wResult = vkCreatePipelineLayout(mDevice, &wPipelineLayoutInfo, nullptr, &mPipelineLayout);
	CHECK_VK_RESULT(wResult, "Graphics Pipeline Layout Creation");

	VkPipelineRenderingCreateInfo wPipelineRenderingInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.pNext = nullptr,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &iColorFormat
	};

	VkDynamicState wDynamicStates[2] = { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT };

	VkPipelineDynamicStateCreateInfo wDynamicState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = 2,
		.pDynamicStates = &wDynamicStates[0]
	};

	VkGraphicsPipelineCreateInfo wCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &wPipelineRenderingInfo,
		.stageCount = (uint32_t)_countof(wShaderStageCreateInfo),
		.pStages = &wShaderStageCreateInfo[0],
		.pVertexInputState = &wVertexInputState,
		.pInputAssemblyState = &wInputAssemblyState,
		.pViewportState = &wViewportState,
		.pRasterizationState = &wRasterizationState,
		.pMultisampleState = &wMultisampleState,
		.pColorBlendState = &wColorBlendState,
		.pDynamicState = &wDynamicState,
		.layout = mPipelineLayout,
		.renderPass = nullptr,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};

	wResult = vkCreateGraphicsPipelines(mDevice, nullptr, 1, &wCreateInfo, nullptr, &mPipeline);
	CHECK_VK_RESULT(wResult, "Graphics Pipeline Creation");

	spdlog::info("Graphics Pipeline Created");
}


