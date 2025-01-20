#include "Renderer.h"

#include <vma/vk_mem_alloc.h>
#include <glm/gtc/type_ptr.hpp>


#include "Core/Logger.h"

#include "UIManager.h"
#include "TextureManager.h"

#include "Scene/StaticMesh.h"
#include "Scene/Camera.h"
#include "Scene/Model.h"

#include "Core/Window.h"
#include "Core/Engine.h"

#include "Graphics/Utils.h"

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
	mObjectsUB->Initialize(mContext->mLogicalDevice.get(), sizeof(ObjectUB) * mMaxNumberMeshes * mContext->GetNumFramesInFlight(), mContext->mAllocator);

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

void Renderer::StartFrame()
{
	mContext->mCompleteFences[mCurrentFrameInFlight]->Wait();
	mContext->mCompleteFences[mCurrentFrameInFlight]->Reset();

	mCurrentSwapchainImageIndex = mContext->mQueue->AcquireNextImage(nullptr, mCurrentFrameInFlight);

	mContext->mDescriptorSetManager->ResetPool(mCurrentFrameInFlight);
}

void Renderer::RenderScene()
{
	VulkanCommandBuffer* wCmd = &mContext->mCmds[mCurrentFrameInFlight];

	FrameUB wFrameUB =
	{
		.ViewMatrix = Engine::Get().GetCamera()->GetViewMatrix(),
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

#ifdef _DEBUG
	CmdDebug::CmdBeginLabel(wCmd->mCmd, "Scene Pass", glm::vec4{ 0.0, 1.0, 0.0, 1.0 });
#endif

	if (Engine::Get().GetModel())
	{
		UpdateObjectsUniformBuffer();
		mViewport->BeginFrame(wCmd->mCmd);
		mMainPass->Begin(wCmd->mCmd, &wFrameResources);
		mMainPass->Draw(wCmd->mCmd, &wFrameResources);
		mMainPass->End(wCmd->mCmd, &wFrameResources);
		mViewport->EndFrame(wCmd->mCmd);
	}

#ifdef _DEBUG
	CmdDebug::CmdEndLabel(wCmd->mCmd);
#endif

	wCmd->End();

	mContext->mQueue->SubmitAsync(wCmd, nullptr);
}

void Renderer::FinishFrame(UIManager* iUIManager)
{
	VulkanCommandBuffer* wCmd = &mContext->mUICmds[mCurrentFrameInFlight];
	FrameResources wFrameResources =
	{
		.mFrameUniformBuffer = mFrameUB.get(),
		.mObjectsUniformBuffer = mObjectsUB.get(),
		.mFrameRenderTarget = mFrameRenderTarget.get(),
		.mViewport = mViewport.get()
	};

	wCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

#ifdef _DEBUG
	CmdDebug::CmdBeginLabel(wCmd->mCmd, "UI Pass", glm::vec4{ 1.0, 0.0, 0.0, 1.0 });
#endif
	mFrameRenderTarget->Transition(wCmd->mCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	mUIPass->Begin(wCmd->mCmd, &wFrameResources);
	iUIManager->Execute();
	mUIPass->Draw(wCmd->mCmd, &wFrameResources);
	mUIPass->End(wCmd->mCmd, &wFrameResources);
	mFrameRenderTarget->Transition(wCmd->mCmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

#ifdef _DEBUG
	CmdDebug::CmdEndLabel(wCmd->mCmd);
#endif

	mContext->mSwapchain->TransitionImageToDraw(wCmd, mCurrentSwapchainImageIndex);

	mContext->BlitImage(wCmd->mCmd, mFrameRenderTarget->mImage, mContext->mSwapchain->mImages[mCurrentSwapchainImageIndex],
		VkOffset3D{ mWindow->GetWidth(), mWindow->GetHeight(), 1 }, VkOffset3D{ mWindow->GetWidth(), mWindow->GetHeight(), 1 });

	mContext->mSwapchain->TransitionImageToPresent(wCmd, mCurrentSwapchainImageIndex);

	wCmd->End();

	mContext->mQueue->SubmitAsync(wCmd, mContext->mCompleteFences[mCurrentFrameInFlight]->mFence, mContext->mCmds[mCurrentFrameInFlight].mCmdSubmitSemaphore->mSemaphore);
}

void Renderer::Present()
{
	VulkanCommandBuffer* wCmd = &mContext->mUICmds[mCurrentFrameInFlight];
	mContext->mQueue->Present(mCurrentSwapchainImageIndex, wCmd->mCmdSubmitSemaphore->mSemaphore);
	mCurrentFrameInFlight = (mCurrentFrameInFlight + 1) % mContext->GetNumFramesInFlight();
}

void Renderer::UpdateObjectsUniformBuffer()
{
	uint32_t wCurrentInFlightIndex = mContext->mQueue->GetCurrentInFlightFrame();

	Model* wModel = Engine::Get().GetModel();
	wModel->SetUniformBufferOffset(0);

	uint32_t wCurrentOffset = 0;
	uint32_t wUniformSize = sizeof(ObjectUB);
	uint32_t wUniformStride = sizeof(ObjectUB) * mContext->GetNumFramesInFlight();
	void* wMappedMem = mObjectsUB->MapMemory(0, 0);

	for (uint32_t i = 0; i < wModel->GetNumMeshes(); i++)
	{
		ObjectUB wObjectUB =
		{
			.ModelMatrix = wModel->GetMesh(i)->GetTransform()->GetMatrix(),
			.HasUV = wModel->GetMesh(i)->isAttributeEnabled(StaticMesh::MeshAttributes::UV),
		};
		
		memcpy((char*)wMappedMem + wCurrentOffset + wCurrentInFlightIndex * wUniformSize, &wObjectUB, sizeof(ObjectUB));
		wModel->GetMesh(i)->SetUniformBufferOffset(wModel->GetUniformBufferOffset() + wCurrentOffset);
		wCurrentOffset += wUniformStride;
	}

	mObjectsUB->UnmapMemory();
}
