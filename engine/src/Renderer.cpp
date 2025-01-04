#include "Renderer.h"
#include "StaticMesh.h"
#include "Window.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

Renderer::Renderer()
{
	mContext = std::make_unique<RenderContext>();
}

void Renderer::Initialize(Window* iWindow)
{
	mWindow = iWindow;
	mContext->Initialize(iWindow);
	mMainPass = std::make_unique<VulkanRenderPass>(mContext->mLogicalDevice->mDevice, mContext->mSwapchain->GetSurfaceCapabilites().currentExtent);

	// Initialize Descriptor Sets Manager
	mDescriptorSetManager = std::make_unique<DescriptorSetManager>(mContext->mLogicalDevice->mDevice, mContext->mSwapchain->GetNumImages());
	CreateDescriptorSetLayouts();

	// Initialize Pipeline Layout Manager
	mPipelineLayoutManager = std::make_unique<PipelineLayoutManager>(mContext->mLogicalDevice->mDevice);
	CreatePipelineLayouts();

	CreateShaders();
	CreatePipelines();
}

void Renderer::UploadMesh(StaticMesh* iMesh)
{
	mContext->mCopyCmd.Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
	iMesh->Upload(&mContext->mCopyCmd, mContext->mLogicalDevice.get(), mContext->mStagingBuffer.get());
	mContext->mCopyCmd.End();

	mContext->mQueue->SubmitSync(&mContext->mCopyCmd);
	Flush();

	spdlog::info("Mesh {:s} uploaded", iMesh->GetName());
}

void Renderer::Render(StaticMesh* iMesh)
{
	uint32_t wCurrentImageIndex = mContext->mQueue->AcquireNextImage();

	VulkanCommandBuffer* wCmd = &mContext->mCmds[wCurrentImageIndex];
	mContext->mFences[wCurrentImageIndex]->Wait();
	mDescriptorSetManager->ResetPool(wCurrentImageIndex);
	mContext->mFences[wCurrentImageIndex]->Reset();
	wCmd->Reset(0);

	VkDescriptorBufferInfo wBufferInfo =
	{
		.buffer = iMesh->GetVertexBuffer()->mBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};

	
	wCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	mContext->mSwapchain->TransitionImageToDraw(wCmd, wCurrentImageIndex);

	std::vector<VkImageView> wBackbuffer{mContext->mSwapchain->mImageViews[wCurrentImageIndex]};

	mMainPass->Begin(wCmd->mCmd, wBackbuffer, nullptr);

	mPipeline->Bind(wCmd->mCmd, mWindow);

	VulkanDescriptorSet wDS = mDescriptorSetManager->AllocateDescriptorSet("main", wCurrentImageIndex);
	wDS.Update(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wBufferInfo);
	
	VkDescriptorSet wDSHandle = wDS.GetHandle();

	vkCmdBindDescriptorSets(wCmd->mCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayoutManager->GetLayout("main"),
		0, 1, &wDSHandle,
		0, nullptr);

	vkCmdDraw(wCmd->mCmd, iMesh->GetNumVertices(), 1, 0, 0);

	mMainPass->End(wCmd->mCmd);

	mContext->mSwapchain->TransitionImageToPresent(wCmd, wCurrentImageIndex);

	wCmd->End();
	
	mContext->mQueue->SubmitAsync(wCmd, mContext->mFences[wCurrentImageIndex]->mFence);
	mContext->mQueue->Present(wCurrentImageIndex, wCmd->mCmdSubmitSemaphore->mSemaphore);
}

void Renderer::Flush()
{
	mContext->mQueue->Flush();
}

void Renderer::CreateDescriptorSetLayouts()
{
	std::vector<DescriptorSetManager::Binding> wBinding =
	{
		{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE}
	};

	mDescriptorSetManager->CreateLayout(wBinding, "main");
}

void Renderer::CreatePipelineLayouts()
{
	std::vector<VkDescriptorSetLayout> wDSLayouts = { mDescriptorSetManager->GetLayout("main") };
	std::vector<VkPushConstantRange> wPCRanges;
	mPipelineLayoutManager->CreateLayout(wDSLayouts, wPCRanges, "main");
}

void Renderer::CreateShaders()
{
	mVS = std::make_unique<VulkanShader>();
	mFS = std::make_unique<VulkanShader>();

	mVS->Initialize(mContext->mLogicalDevice->mDevice, "shaders/bin/basic.vert.spv");
	mFS->Initialize(mContext->mLogicalDevice->mDevice, "shaders/bin/basic.frag.spv");
}

void Renderer::CreatePipelines()
{
	VulkanGraphicsPipeline::PipelineInfo wPipeline{};
	VulkanGraphicsPipeline::DefaultPipelineConfigInfo(wPipeline);

	wPipeline.pipelineLayout = mPipelineLayoutManager->GetLayout("main");

	wPipeline.renderingInfo.pColorAttachmentFormats = &mContext->mSwapchain->mColorFormat;
	wPipeline.renderingInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
	wPipeline.renderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

	mPipeline = std::make_unique<VulkanGraphicsPipeline>();
	mPipeline->Initialize(mContext->mLogicalDevice->mDevice, wPipeline, mVS->mShader, mFS->mShader);
}
