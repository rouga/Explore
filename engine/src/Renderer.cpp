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

	VkImageSubresourceRange wSubresourceRange =
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

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

	VkImageMemoryBarrier wPresentToClear =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mContext->mSwapchain->mImages[wCurrentImageIndex],
		.subresourceRange = wSubresourceRange
	};

	VkImageMemoryBarrier wToPresent =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mContext->mSwapchain->mImages[wCurrentImageIndex],
		.subresourceRange = wSubresourceRange
	};

	const VkRenderingAttachmentInfo wColorAttachmentInfo =
	{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = mContext->mSwapchain->mImageViews[wCurrentImageIndex],
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = wClearColor,
	};

	VkRect2D wRenderArea =
	{
		.offset = { 0, 0},
		.extent = { (uint32_t)mWindow->GetWidth(), (uint32_t)mWindow->GetHeight() }
	};

	const VkRenderingInfo render_info
	{
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = wRenderArea,
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &wColorAttachmentInfo
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

	vkCmdPipelineBarrier(wCmd->mCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &wPresentToClear);

	vkCmdBeginRendering(wCmd->mCmd, &render_info);

	vkCmdBindPipeline(wCmd->mCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mContext->mPipeline->mPipeline);
	vkCmdSetViewport(wCmd->mCmd, 0, 1, &wViewport);
	vkCmdSetScissor(wCmd->mCmd, 0, 1, &wScissor);

	mContext->mDescriptorSets[wCurrentImageIndex]->Update(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wBufferInfo);
	VkDescriptorSet wDS = mContext->mDescriptorSets[wCurrentImageIndex]->GetHandle();
	vkCmdBindDescriptorSets(wCmd->mCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mContext->mPipeline->mPipelineLayout,
		0, 1, &wDS,
		0, nullptr);

	vkCmdDraw(wCmd->mCmd, 3, 1, 0, 0);

	vkCmdEndRendering(wCmd->mCmd);

	vkCmdPipelineBarrier(wCmd->mCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &wToPresent);

	wCmd->End();
	
	mContext->mQueue->SubmitAsync(wCmd, mContext->mFences[wCurrentImageIndex]->mFence);
	mContext->mQueue->Present(wCurrentImageIndex, wCmd->mCmdSubmitSemaphore->mSemaphore);
}

void Renderer::Flush()
{
	mContext->mQueue->Flush();
}
