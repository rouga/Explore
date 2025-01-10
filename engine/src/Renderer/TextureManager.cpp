#include "TextureManager.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

TextureManager& TextureManager::Get()
{
	static TextureManager sInstance;
	return sInstance;
}


void TextureManager::Initialize(RenderContext* iContext)
{
	mContext = iContext;
	sGridTexture = std::make_shared<VulkanImage>(mContext->mLogicalDevice->mDevice, mContext->mAllocator);
	CreateTexture("resources/textures/grid.png", sGridTexture.get());
}

void TextureManager::Shutdown()
{
	sGridTexture->FreeGPU();
}

std::shared_ptr<VulkanImage> TextureManager::AddTexture(std::string iPath)
{
	// Check if texture exists in the cache
	if (auto wCachedTexture = mTextureCache[iPath].lock())
	{
		return wCachedTexture;
	}

	std::shared_ptr<VulkanImage> wTexture = std::make_shared<VulkanImage>(mContext->mLogicalDevice->mDevice, mContext->mAllocator);
	mTextureCache[iPath] = wTexture; // Store a weak_ptr in the cache
	mTexturePending.push_back(iPath);
	return wTexture;
}

void TextureManager::LoadPending()
{
	if(mTexturePending.size() > 0)
	{
		for(auto& wTexturePath : mTexturePending)
		{
			VulkanImage* wTexture = mTextureCache[wTexturePath].lock().get();
			CreateTexture(wTexturePath, wTexture);
		}

		mTexturePending.clear();
	}
}

void TextureManager::CreateTexture(std::string iPath, VulkanImage* iTexture)
{
	int wWidth, wHeight, wChannelCount;
	stbi_uc* wPixelData = stbi_load(iPath.c_str(), &wWidth, &wHeight, &wChannelCount, STBI_rgb_alpha);

	if (!wPixelData)
	{
		spdlog::error("Failed to load texture: {:s}", iPath);
		exit(EXIT_FAILURE);
	}

	VkDeviceSize wImageSize = wWidth * wHeight * 4;

	VulkanImage::ImageConfig wImageConfig{};
	wImageConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
	wImageConfig.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	wImageConfig.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	iTexture->Initialize(VkExtent3D{ (uint32_t)wWidth, (uint32_t)wHeight,1 }, wImageConfig);

	VulkanCommandBuffer* iCopyCmd = &mContext->mCopyCmd;
	iCopyCmd->Reset(0);
	iCopyCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	iTexture->Transition(iCopyCmd->mCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	void* wMappedMem = mContext->mStagingBuffer->MapMemory(0, 0);
	memcpy((char*)wMappedMem, wPixelData, wImageSize);
	mContext->mStagingBuffer->UnmapMemory();

	iTexture->UploadData(iCopyCmd, mContext->mStagingBuffer.get(), wImageSize, 0, VkExtent3D{ (uint32_t)wWidth, (uint32_t)wHeight,1 });

	iTexture->Transition(iCopyCmd->mCmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	iCopyCmd->End();
	mContext->mQueue->SubmitSync(iCopyCmd, mContext->mCopyFence.get());
}
