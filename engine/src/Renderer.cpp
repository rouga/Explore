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

	mFrameUB = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	mFrameUB->Initialize(mContext->mLogicalDevice.get(), sizeof(FrameUB), mContext->mAllocator);

	std::vector<DescriptorSetManager::Binding> wFrameUBBinding =
	{
		{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE}
	};

	mContext->mDescriptorSetManager->CreateLayout(wFrameUBBinding, "FrameUB");

	mMainPass = std::make_unique<MainPass>(mContext.get());
	mMainPass->Setup(nullptr);
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
	mContext->mDescriptorSetManager->ResetPool(wCurrentImageIndex);
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

	VkDescriptorBufferInfo wFrameUBInfo =
	{
		.buffer = mFrameUB->mBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};
	
	VulkanDescriptorSet wFrameUBDS = mContext->mDescriptorSetManager->AllocateDescriptorSet("FrameUB", wCurrentImageIndex);
	wFrameUBDS.Update(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &wFrameUBInfo);
	VkDescriptorSet wFrameUBDSHandle = wFrameUBDS.GetHandle();
	VkDescriptorSet wDSList[] = { wFrameUBDSHandle };

	wCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	mContext->mSwapchain->TransitionImageToDraw(wCmd, wCurrentImageIndex);

	mMainPass->Begin(wCmd->mCmd);

	vkCmdBindDescriptorSets(wCmd->mCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mContext->mPipelineLayoutManager->GetLayout("main"),
		0, _countof(wDSList), wDSList,
		0, nullptr);

	mMainPass->Draw(wCmd->mCmd);
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

void Renderer::Resize(int iWidth, int iHeight)
{
	Flush();
	mContext->Resize(iWidth, iHeight);
}
