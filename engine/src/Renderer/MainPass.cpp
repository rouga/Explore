#include "Renderer/MainPass.h"

#include "Graphics/RenderContext.h"
#include "Core/Engine.h"
#include "Core/Window.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

MainPass::MainPass(RenderContext* iContext)
	:RenderPass(iContext)
{
}

void MainPass::Setup(VkCommandBuffer iCmd)
{
	mRenderPass = std::make_unique<VulkanRenderPass>(mContext->mLogicalDevice->mDevice);

	// Setup Descriptor set layouts
	std::vector<DescriptorSetManager::Binding> wBinding =
	{
		{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE},
		{1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE},
	};

	mContext->mDescriptorSetManager->CreateLayout(wBinding, "ObjectDS");

	// Setup Pipeline layout
	std::vector<VkDescriptorSetLayout> wDSLayouts = { mContext->mDescriptorSetManager->GetLayout("FrameUB"), mContext->mDescriptorSetManager->GetLayout("ObjectDS") };
	std::vector<VkPushConstantRange> wPCRanges;
	mContext->mPipelineLayoutManager->CreateLayout(wDSLayouts, wPCRanges, "main");

	// Create Pipeline
	mVS = std::make_unique<VulkanShader>();
	mFS = std::make_unique<VulkanShader>();

	mVS->Initialize(mContext->mLogicalDevice->mDevice, "shaders/bin/basic.vert.spv");
	mFS->Initialize(mContext->mLogicalDevice->mDevice, "shaders/bin/basic.frag.spv");

	VulkanGraphicsPipeline::PipelineInfo wPipeline{};
	VulkanGraphicsPipeline::DefaultPipelineConfigInfo(wPipeline);

	wPipeline.pipelineLayout = mContext->mPipelineLayoutManager->GetLayout("main");

	wPipeline.renderingInfo.pColorAttachmentFormats = &mContext->mSwapchain->mColorFormat;
	wPipeline.renderingInfo.depthAttachmentFormat = mContext->mDepthBuffer->GetFormat();
	wPipeline.renderingInfo.stencilAttachmentFormat = mContext->mDepthBuffer->GetFormat();

	mPipeline = std::make_unique<VulkanGraphicsPipeline>();
	mPipeline->Initialize(mContext->mLogicalDevice->mDevice, wPipeline, mVS->mShader, mFS->mShader);

	spdlog::info("Main pass setup completed.");
}

void MainPass::Begin(VkCommandBuffer iCmd)
{
	uint32_t wCurrentImageIndex = mContext->mQueue->GetCurrentImageIndex();
	std::vector<VkImageView> wBackbuffer{ mContext->mSwapchain->mImageViews[wCurrentImageIndex] };

	mRenderPass->Begin(iCmd, wBackbuffer, mContext->mDepthBuffer->mImageView, 
		VkExtent2D{(uint32_t)Engine::Get().GetWindow()->GetWidth(), (uint32_t)Engine::Get().GetWindow()->GetHeight()});

	mPipeline->Bind(iCmd, mContext->mWindow);
}

void MainPass::Draw(VkCommandBuffer iCmd)
{
	uint32_t wCurrentImageIndex = mContext->mQueue->GetCurrentImageIndex();

	for (uint32_t i = 0; i < Engine::Get().GetModel()->GetNumMeshes(); i++)
	{
		VkDescriptorBufferInfo wVertexBufferInfo =
		{
			.buffer = Engine::Get().GetModel()->GetMesh(i)->GetVertexBuffer()->mBuffer,
			.offset = 0,
			.range = VK_WHOLE_SIZE
		};
		VkDescriptorBufferInfo wIndexBufferInfo =
		{
			.buffer = Engine::Get().GetModel()->GetMesh(i)->GeIndexBuffer()->mBuffer,
			.offset = 0,
			.range = VK_WHOLE_SIZE
		};

		VulkanDescriptorSet wDS = mContext->mDescriptorSetManager->AllocateDescriptorSet("ObjectDS", wCurrentImageIndex);
		wDS.Update(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wVertexBufferInfo);
		wDS.Update(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wIndexBufferInfo);

		VkDescriptorSet wDSHandle = wDS.GetHandle();

		VkDescriptorSet wDSList[] = { wDSHandle };

		vkCmdBindDescriptorSets(iCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mContext->mPipelineLayoutManager->GetLayout("main"),
			1, _countof(wDSList), wDSList,
			0, nullptr);

		vkCmdDraw(iCmd, Engine::Get().GetModel()->GetMesh(i)->GetIndexCount(), 1, 0, 0);
	}
}

void MainPass::End(VkCommandBuffer iCmd)
{
	mRenderPass->End(iCmd);
}
