#include "Renderer.h"
#include "StaticMesh.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

Renderer::Renderer()
{
	mContext = std::make_unique<RenderContext>();
}

void Renderer::Initialize(Window* iWindow)
{
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

void Renderer::Render()
{
	uint32_t wCurrentImageIndex = mContext->mQueue->AcquireNextImage();
	mContext->mQueue->SubmitAsync(&mContext->mCmds[wCurrentImageIndex]);
	mContext->mQueue->Present(wCurrentImageIndex, mContext->mCmds[wCurrentImageIndex].mCmdFinishSemaphore->mSemaphore);
}
