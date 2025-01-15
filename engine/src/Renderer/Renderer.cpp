#include "Renderer.h"

#include <vma/vk_mem_alloc.h>
#include <glm/gtc/type_ptr.hpp>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "TextureManager.h"

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

	TextureManager::Get().Initialize(mContext.get());

	mFrameUB = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	mFrameUB->Initialize(mContext->mLogicalDevice.get(), sizeof(FrameUB), mContext->mAllocator);

	mObjectsUB = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	mObjectsUB->Initialize(mContext->mLogicalDevice.get(), sizeof(ObjectUB) * mMaxNumberMeshes * mContext->mSwapchain->GetNumImages(), mContext->mAllocator);

	std::vector<DescriptorSetManager::Binding> wFrameUBBinding =
	{
		{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE}
	};

	mContext->mDescriptorSetManager->CreateLayout(wFrameUBBinding, "FrameUB");

	mViewport = std::make_unique<Viewport>(mContext.get());
	mViewport->Initialize();

	VulkanImage::ImageConfig wImageConfig{};
	wImageConfig.format = VK_FORMAT_R8G8B8A8_UNORM;
	wImageConfig.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	wImageConfig.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	mFrameRenderTarget = std::make_unique<VulkanImage>(mContext->mLogicalDevice->mDevice, mContext->mAllocator);
	mFrameRenderTarget->Initialize(VkExtent3D{
		.width = (uint32_t)mContext->mWindow->GetWidth(),
		.height = (uint32_t)mContext->mWindow->GetHeight(),
		.depth = 1
		}, wImageConfig);

	FrameResources wFrameResources =
	{
		.mFrameRenderTarget = mFrameRenderTarget.get(),
		.mViewport = mViewport.get()
	};

	mMainPass = std::make_unique<MainPass>(mContext.get());
	mMainPass->Setup(nullptr, &wFrameResources);

	mUIPass = std::make_unique<UIPass>(mContext.get());
	mUIPass->Setup(nullptr, &wFrameResources);
}

void Renderer::UploadGeometry(Model* iModel)
{
	VulkanCommandBuffer* iCopyCmd = &mContext->mCopyCmd;

	for(uint32_t i = 0; i < iModel->GetNumMeshes(); i++)
	{
		iCopyCmd->Reset(0);
		iCopyCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		iModel->GetMesh(i)->UploadGeometry(iCopyCmd, mContext.get());
		iCopyCmd->End();
		mContext->mQueue->SubmitSync(iCopyCmd, mContext->mCopyFence.get());
	}

	TextureManager::Get().LoadPending();
}

void Renderer::Render()
{
	uint32_t wCurrentImageIndex = mContext->mQueue->AcquireNextImage(nullptr);

	VulkanCommandBuffer* wCmd = &mContext->mCmds[wCurrentImageIndex];
	mContext->mCompleteFences[wCurrentImageIndex]->Wait();
	mContext->mCompleteFences[wCurrentImageIndex]->Reset();
	wCmd->Reset(0);
	mContext->mDescriptorSetManager->ResetPool(wCurrentImageIndex);

	Engine::Get().GetUI()->BeginFrame();

	FrameUB wFrameUB =
	{
		.ViewMatrix = Engine::Get().GetCamera()->getViewMatrix(),
		.ProjectionMatrix = Engine::Get().GetCamera()->GetProjectionMatrix(),
	};

	void* wMappedMem = mFrameUB->MapMemory(0, 0);
	memcpy(wMappedMem, &wFrameUB, sizeof(FrameUB));
	mFrameUB->UnmapMemory();

	FrameResources wFrameResources =
	{
		.FrameUB = wFrameUB,
		.mFrameUniformBuffer = mFrameUB.get(),
		.mObjectsUniformBuffer = mObjectsUB.get(),
		.mFrameRenderTarget = mFrameRenderTarget.get(),
		.mViewport = mViewport.get()
	};

	wCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	if(Engine::Get().GetModel())
	{
		UpdateObjectsUniformBuffer();
		mViewport->BeginFrame(wCmd->mCmd);
		mMainPass->Begin(wCmd->mCmd, &wFrameResources);
		mMainPass->Draw(wCmd->mCmd, &wFrameResources);
		mMainPass->End(wCmd->mCmd, &wFrameResources);
		mViewport->EndFrame(wCmd->mCmd);
	}

	mFrameRenderTarget->Transition(wCmd->mCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	mUIPass->Begin(wCmd->mCmd, &wFrameResources);
	mUIPass->Draw(wCmd->mCmd, &wFrameResources);
	mUIPass->End(wCmd->mCmd, &wFrameResources);
	mFrameRenderTarget->Transition(wCmd->mCmd,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	mContext->mSwapchain->TransitionImageToDraw(wCmd, wCurrentImageIndex);

	mContext->BlitImage(wCmd->mCmd, mFrameRenderTarget->mImage, mContext->mSwapchain->mImages[wCurrentImageIndex],
		VkOffset3D{mWindow->GetWidth(), mWindow->GetHeight(), 1}, VkOffset3D{ mWindow->GetWidth(), mWindow->GetHeight(), 1 });

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
	mFrameRenderTarget->Resize(VkExtent3D{(uint32_t)iWidth, (uint32_t)iHeight, 1});
	mContext->Resize(iWidth, iHeight);
}

void Renderer::UpdateObjectsUniformBuffer()
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
		ObjectUB wObjectUB =
		{
			.ModelMatrix = wModel->GetMesh(i)->GetTransform()->GetMatrix(),
			.HasUV = wModel->GetMesh(i)->isAttributeEnabled(StaticMesh::MeshAttributes::UV),
		};
		
		memcpy((char*)wMappedMem + wCurrentOffset + wCurrentImageIndex * wUniformSize, &wObjectUB, sizeof(ObjectUB));
		wModel->GetMesh(i)->SetUniformBufferOffset(wModel->GetUniformBufferOffset() + wCurrentOffset);
		wCurrentOffset += wUniformStride;
	}

	mObjectsUB->UnmapMemory();
}
