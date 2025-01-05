#include "Renderer.h"

#include <vma/vk_mem_alloc.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "StaticMesh.h"
#include "Window.h"
#include "Camera.h"
#include "Engine.h"

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

	mFrameUB = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	mFrameUB->Initialize(mContext->mLogicalDevice.get(), sizeof(FrameUB), mContext->mAllocator);
}

void Renderer::UploadMesh(StaticMesh* iMesh)
{
	VulkanCommandBuffer* iCopyCmd = &mContext->mCopyCmd;
	iCopyCmd->Reset(0);
	iCopyCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	iMesh->Upload(iCopyCmd, mContext.get());
	iCopyCmd->End();

	mContext->mQueue->SubmitSync(iCopyCmd, mContext->mCopyFence->mFence);
	mContext->mCopyFence->Wait();
	mContext->mCopyFence->Reset();

	spdlog::info("Mesh {:s} uploaded", iMesh->GetName());
}

void Renderer::Render()
{
	uint32_t wCurrentImageIndex = mContext->mQueue->AcquireNextImage();

	VulkanCommandBuffer* wCmd = &mContext->mCmds[wCurrentImageIndex];
	mContext->mCompleteFences[wCurrentImageIndex]->Wait();
	mDescriptorSetManager->ResetPool(wCurrentImageIndex);
	mContext->mCompleteFences[wCurrentImageIndex]->Reset();
	wCmd->Reset(0);

	FrameUB wFrameUB =
	{
		.ViewMatrix = Engine::Get().GetCamera()->getViewMatrix(),
		.ProjectionMatrix = Engine::Get().GetCamera()->GetProjectionMatrix(),
	};

	void* wMappedMem = mFrameUB->MapMemory(0, 0);
	memcpy(wMappedMem, &wFrameUB, sizeof(FrameUB));
	mFrameUB->UnmapMemory();

	VkDescriptorBufferInfo wVertexBufferInfo =
	{
		.buffer = Engine::Get().GetMesh()->GetVertexBuffer()->mBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};
	VkDescriptorBufferInfo wIndexBufferInfo =
	{
		.buffer = Engine::Get().GetMesh()->GeIndexBuffer()->mBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};

	VkDescriptorBufferInfo wFrameUBInfo =
	{
		.buffer = mFrameUB->mBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};
	
	VulkanDescriptorSet wFrameUBDS = mDescriptorSetManager->AllocateDescriptorSet("FrameUB", wCurrentImageIndex);
	wFrameUBDS.Update(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &wFrameUBInfo);

	wCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	mContext->mSwapchain->TransitionImageToDraw(wCmd, wCurrentImageIndex);

	std::vector<VkImageView> wBackbuffer{mContext->mSwapchain->mImageViews[wCurrentImageIndex]};

	mMainPass->Begin(wCmd->mCmd, wBackbuffer, mContext->mDepthBuffer->mImageView);

	mPipeline->Bind(wCmd->mCmd, mWindow);

	VulkanDescriptorSet wDS = mDescriptorSetManager->AllocateDescriptorSet("ObjectDS", wCurrentImageIndex);
	wDS.Update(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wVertexBufferInfo);
	wDS.Update(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wIndexBufferInfo);

	VkDescriptorSet wDSHandle = wDS.GetHandle();
	VkDescriptorSet wFrameUBDSHandle = wFrameUBDS.GetHandle();

	VkDescriptorSet wDSList[] = { wFrameUBDSHandle,wDSHandle};

	vkCmdBindDescriptorSets(wCmd->mCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayoutManager->GetLayout("main"),
		0, _countof(wDSList), wDSList,
		0, nullptr);

	vkCmdDraw(wCmd->mCmd, Engine::Get().GetMesh()->GetIndexCount(), 1, 0, 0);

	mMainPass->End(wCmd->mCmd);

	mContext->mSwapchain->TransitionImageToPresent(wCmd, wCurrentImageIndex);

	wCmd->End();
	
	mContext->mQueue->SubmitAsync(wCmd, mContext->mCompleteFences[wCurrentImageIndex]->mFence);
	mContext->mQueue->Present(wCurrentImageIndex, wCmd->mCmdSubmitSemaphore->mSemaphore);
}

void Renderer::Flush()
{
	mContext->mQueue->Flush();
}

void Renderer::CreateDescriptorSetLayouts()
{
	std::vector<DescriptorSetManager::Binding> wFrameUBBinding =
	{
		{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE}
	};

	std::vector<DescriptorSetManager::Binding> wBinding =
	{
		{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE},
		{1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE},
	};

	mDescriptorSetManager->CreateLayout(wFrameUBBinding, "FrameUB");
	mDescriptorSetManager->CreateLayout(wBinding, "ObjectDS");
}

void Renderer::CreatePipelineLayouts()
{
	std::vector<VkDescriptorSetLayout> wDSLayouts = { mDescriptorSetManager->GetLayout("FrameUB"), mDescriptorSetManager->GetLayout("ObjectDS") };
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
	wPipeline.renderingInfo.depthAttachmentFormat = mContext->mDepthBuffer->GetFormat();
	wPipeline.renderingInfo.stencilAttachmentFormat = mContext->mDepthBuffer->GetFormat();

	mPipeline = std::make_unique<VulkanGraphicsPipeline>();
	mPipeline->Initialize(mContext->mLogicalDevice->mDevice, wPipeline, mVS->mShader, mFS->mShader);
}
