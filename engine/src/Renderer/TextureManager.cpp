#include "TextureManager.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include <stb/stb_image.h>

TextureManager::TextureManager(RenderContext* iContext)
	:mContext(iContext)
{
}

std::shared_ptr<VulkanImage> TextureManager::LoadTexture(std::string iPath)
{
	// Check if texture exists in the cache
	if (auto wCachedTexture = mTextureCache[iPath].lock()) 
	{
		return wCachedTexture;
	}

	// Create a new texture if not found or expired
	auto newTexture = CreateTexture(iPath);
	mTextureCache[iPath] = newTexture; // Store a weak_ptr in the cache
	return newTexture;
}

std::shared_ptr<VulkanImage> TextureManager::CreateTexture(std::string iPath)
{
	int wWidth, wHeight, wChannelCount;
	stbi_uc* wPixelData = stbi_load(iPath.c_str(), &wWidth, &wHeight, &wChannelCount, STBI_rgb_alpha);

	if (!wPixelData)
	{
		spdlog::error("Failed to load texture: {:s}", iPath);
		exit(EXIT_FAILURE);
	}

	VkDeviceSize wImageSize = wWidth * wHeight * wChannelCount;
	
	VulkanImage::ImageConfig wImageConfig{};
	wImageConfig.format = wChannelCount == 4 ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8_SRGB;
	wImageConfig.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	wImageConfig.usage = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	std::shared_ptr<VulkanImage> wTexture = std::make_shared<VulkanImage>(mContext->mLogicalDevice->mDevice, mContext->mAllocator);
	wTexture->Initialize(VkExtent3D{(uint32_t)wWidth, (uint32_t)wHeight,1}, wImageConfig);

	VulkanCommandBuffer* iCopyCmd = &mContext->mCopyCmd;
	iCopyCmd->Reset(0);
	iCopyCmd->Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
	wTexture->Transition(iCopyCmd->mCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	void* wMappedMem = mContext->mStagingBuffer->MapMemory(0, 0);
	memcpy((char*)wMappedMem, wPixelData, wImageSize);
	mContext->mStagingBuffer->UnmapMemory();

	wTexture->UploadData(iCopyCmd, mContext->mStagingBuffer.get(), wImageSize,0, VkExtent3D{ (uint32_t)wWidth, (uint32_t)wHeight,1 });

	wTexture->Transition(iCopyCmd->mCmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	iCopyCmd->End();
	mContext->mQueue->SubmitSync(iCopyCmd, mContext->mCopyFence.get());

	return wTexture;
}
