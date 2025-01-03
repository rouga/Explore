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
}

void Renderer::UploadMesh(StaticMesh* iMesh)
{
	mContext->mCopyCmd.Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
	iMesh->Upload(&mContext->mCopyCmd, mContext->mLogicalDevice.get(), mContext->mStagingBuffer.get());
	mContext->mCopyCmd.End();

	mContext->mQueue->SubmitSync(&mContext->mCopyCmd);
	mContext->mQueue->Flush();

	spdlog::info("Mesh {:s} uploaded", iMesh->GetName());
}

void Renderer::Render(StaticMesh* iMesh)
{
	uint32_t wCurrentImageIndex = mContext->mQueue->AcquireNextImage();

	VulkanCommandBuffer* wCmd = &mContext->mCmds[wCurrentImageIndex];

	VkClearColorValue wClearColor = { 1.f, 0.0f, 0.0f, 1.f };

	VkViewport wViewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)mWindow->GetWidth(),
		.height = (float)mWindow->GetHeight(),
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
			.width = (uint32_t)mWindow->GetWidth(),
			.height = (uint32_t)mWindow->GetHeight()
		}
	};

	VkDescriptorBufferInfo wBufferInfo =
	{
		.buffer = iMesh->GetVertexBuffer()->mBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};

	mContext->mFences[wCurrentImageIndex]->Wait();
	mContext->mFences[wCurrentImageIndex]->Reset();
	wCmd->Reset(0);
	
	wCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	mContext->mSwapchain->TransitionImageToDraw(wCmd, wCurrentImageIndex);

	std::vector<VkImageView> wBackbuffer{mContext->mSwapchain->mImageViews[wCurrentImageIndex]};

	mMainPass->Begin(wCmd->mCmd, wBackbuffer, nullptr);

	vkCmdBindPipeline(wCmd->mCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mContext->mPipeline->mPipeline);
	vkCmdSetViewport(wCmd->mCmd, 0, 1, &wViewport);
	vkCmdSetScissor(wCmd->mCmd, 0, 1, &wScissor);

	mContext->mDescriptorSets[wCurrentImageIndex]->Update(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wBufferInfo);
	
	VkDescriptorSet wDS = mContext->mDescriptorSets[wCurrentImageIndex]->GetHandle();

	vkCmdBindDescriptorSets(wCmd->mCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mContext->mPipeline->mPipelineLayout,
		0, 1, &wDS,
		0, nullptr);

	vkCmdDraw(wCmd->mCmd, 3, 1, 0, 0);

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
