#pragma once

#include <memory>
#include <unordered_map>

#include "Graphics/RenderContext.h"

class TextureManager
{
public:
	// Get the singleton instance
	static TextureManager& Get();

	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	void Initialize(RenderContext* iContext);
	void Shutdown();

	std::shared_ptr<VulkanImage> AddTexture(const std::string& iPath);
	void DereferenceTexture(const std::string& iPath);

	void LoadPending();
	
	std::shared_ptr<VulkanImage> mGridTexture = nullptr;

private:
	TextureManager() = default;
	void CreateTexture(std::string iPath, VulkanImage* iTexture);
	
	std::unordered_map<std::string, std::weak_ptr<VulkanImage>> mTextureCache; // Map for loaded textures
	std::vector<std::string> mTexturePending; // Map for loaded textures
	RenderContext* mContext = nullptr;
};