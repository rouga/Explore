#include "VulkanGraphicsPipeline.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Core/Window.h"
#include "Utils.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline()
{

}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	vkDestroyPipeline(mDevice, mPipeline, nullptr);
}

void VulkanGraphicsPipeline::Initialize(VkDevice iDevice, const PipelineInfo iPipelineInfo ,const VkShaderModule iVS, VkShaderModule iFS)
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

	VkGraphicsPipelineCreateInfo wCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &iPipelineInfo.renderingInfo,
		.stageCount = (uint32_t)_countof(wShaderStageCreateInfo),
		.pStages = &wShaderStageCreateInfo[0],
		.pVertexInputState = &iPipelineInfo.vertexInputInfo,
		.pInputAssemblyState = &iPipelineInfo.inputAssemblyInfo,
		.pViewportState = &iPipelineInfo.viewportInfo,
		.pRasterizationState = &iPipelineInfo.rasterizationInfo,
		.pMultisampleState = &iPipelineInfo.multisampleInfo,
		.pDepthStencilState = &iPipelineInfo.depthStencilInfo,
		.pColorBlendState = &iPipelineInfo.colorBlendInfo,
		.pDynamicState = &iPipelineInfo.dynamicStateInfo,
		.layout = iPipelineInfo.pipelineLayout,
		.renderPass = nullptr,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};

	VkResult wResult = vkCreateGraphicsPipelines(mDevice, nullptr, 1, &wCreateInfo, nullptr, &mPipeline);
	CHECK_VK_RESULT(wResult, "Graphics Pipeline Creation");

	spdlog::info("Graphics Pipeline Created");
}

void VulkanGraphicsPipeline::Bind(VkCommandBuffer iCmd, Window* iWindow)
{
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

	vkCmdBindPipeline(iCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
	vkCmdSetViewport(iCmd, 0, 1, &wViewport);
	vkCmdSetScissor(iCmd, 0, 1, &wScissor);
}

void VulkanGraphicsPipeline::Bind(VkCommandBuffer iCmd)
{
	vkCmdBindPipeline(iCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
}

void VulkanGraphicsPipeline::DefaultPipelineConfigInfo(PipelineInfo& iPipelineInfo)
{
	iPipelineInfo.renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	iPipelineInfo.renderingInfo.pNext = nullptr;
	iPipelineInfo.renderingInfo.colorAttachmentCount = 1;

	iPipelineInfo.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	iPipelineInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	iPipelineInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	iPipelineInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	iPipelineInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	iPipelineInfo.viewportInfo.viewportCount = 1;
	iPipelineInfo.viewportInfo.pViewports = nullptr;
	iPipelineInfo.viewportInfo.scissorCount = 1;
	iPipelineInfo.viewportInfo.pScissors = nullptr;

	iPipelineInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	iPipelineInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
	iPipelineInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	iPipelineInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	iPipelineInfo.rasterizationInfo.lineWidth = 1.0f;
	iPipelineInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	iPipelineInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	iPipelineInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

	iPipelineInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	iPipelineInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
	iPipelineInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	iPipelineInfo.colorBlendAttachment.blendEnable = VK_FALSE;
	iPipelineInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	iPipelineInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	iPipelineInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
	iPipelineInfo.colorBlendInfo.attachmentCount = 1;
	iPipelineInfo.colorBlendInfo.pAttachments = &iPipelineInfo.colorBlendAttachment;

	iPipelineInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	iPipelineInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
	iPipelineInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
	iPipelineInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	iPipelineInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	iPipelineInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;

	iPipelineInfo.dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	iPipelineInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	iPipelineInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(iPipelineInfo.dynamicStates.size());
	iPipelineInfo.dynamicStateInfo.pDynamicStates = iPipelineInfo.dynamicStates.data();
}


