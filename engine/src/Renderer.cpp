#include "Renderer.h"


Renderer::Renderer()
{
	mContext = std::make_unique<RenderContext>();
}

void Renderer::Initialize(Window* iWindow)
{
	mContext->Initialize(iWindow);
}

void Renderer::Render()
{
	uint32_t wCurrentImageIndex = mContext->mQueue->AcquireNextImage();
	mContext->mQueue->SubmitAsync(&mContext->mCmds[wCurrentImageIndex]);
	mContext->mQueue->Present(wCurrentImageIndex, mContext->mCmds[wCurrentImageIndex].mCmdFinishSemaphore->mSemaphore);
}
