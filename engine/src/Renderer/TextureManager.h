#pragma once

#include <memory>
#include <unordered_map>

#include "Graphics/RenderContext.h"

class TextureManager
{
public:
	TextureManager(RenderContext* iContext);

	std::shared_ptr<VulkanImage> LoadTexture(std::string iPath);
	
private:
	std::shared_ptr<VulkanImage> CreateTexture(std::string iPath);
	std::unordered_map<std::string, std::weak_ptr<VulkanImage>> mTextureCache; // Map for loaded textures

	RenderContext* mContext = nullptr;
};