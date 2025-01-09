#include "Renderer.h"

#include <vma/vk_mem_alloc.h>
#include <glm/gtc/type_ptr.hpp>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Scene/StaticMesh.h"
#include "Scene/Camera.h"
#include "Scene/Model.h"

#include "Core/Window.h"
#include "Core/Engine.h"

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

	mObjectsUB = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	mObjectsUB->Initialize(mContext->mLogicalDevice.get(), sizeof(ObjectUB) * mMaxNumberMeshes * mContext->mSwapchain->GetNumImages(), mContext->mAllocator);

	std::vector<DescriptorSetManager::Binding> wFrameUBBinding =
	{
		{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE}
	};

	mContext->mDescriptorSetManager->CreateLayout(wFrameUBBinding, "FrameUB");

	mMainPass = std::make_unique<MainPass>(mContext.get());
	mMainPass->Setup(nullptr);
}

void Renderer::UploadModel(Model* iModel)
{
	VulkanCommandBuffer* iCopyCmd = &mContext->mCopyCmd;
	iCopyCmd->Reset(0);
	iCopyCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	for(uint32_t i = 0; i < iModel->GetNumMeshes(); i++)
	{
		iModel->GetMesh(i)->Upload(iCopyCmd, mContext.get());
	}
	
	iCopyCmd->End();

	mContext->mQueue->SubmitSync(iCopyCmd, mContext->mCopyFence->mFence);
	mContext->mCopyFence->Wait();
	mContext->mCopyFence->Reset();
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

	FillUniformBuffer();

	FrameResources wFrameResources =
	{
		.FrameUB = wFrameUB,
		.mFrameUniformBuffer = mFrameUB.get(),
		.mObjectsUniformBuffer = mObjectsUB.get()
	};

	wCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	mContext->mSwapchain->TransitionImageToDraw(wCmd, wCurrentImageIndex);

	mMainPass->Begin(wCmd->mCmd);
	mMainPass->Draw(wCmd->mCmd, &wFrameResources);
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

void Renderer::FillUniformBuffer()
{
	uint32_t wCurrentImageIndex = mContext->mQueue->GetCurrentImageIndex();

	Model* wModel = Engine::Get().GetModel();
	wModel->SetUniformBufferOffset(0);

	uint32_t wCurrentOffset = 0;
	uint32_t wUniformSize = sizeof(ObjectUB);
	uint32_t wUniformStride = sizeof(ObjectUB) * mContext->mSwapchain->GetNumImages();
	void* wMappedMem = mObjectsUB->MapMemory(0, 0);

	for (uint32_t i = 0; i < wModel->GetNumMeshes(); i++)
	{
		const glm::f32* wMatrix = glm::value_ptr(wModel->GetMesh(i)->GetTransform()->GetMatrix());
		memcpy((char*)wMappedMem + wCurrentOffset + wCurrentImageIndex * wUniformSize, wMatrix, sizeof(ObjectUB));
		wModel->GetMesh(i)->SetUniformBufferOffset(wModel->GetUniformBufferOffset() + wCurrentOffset);
		wCurrentOffset += wUniformStride;
	}

	mObjectsUB->UnmapMemory();
}
