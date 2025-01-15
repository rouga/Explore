#pragma once

#include <imgui.h>

#include "Graphics/VulkanImage.h"

class RenderContext;

class Viewport
{
public:
	Viewport(RenderContext* iContext);
	~Viewport();

	void Initialize();

	void BeginFrame(VkCommandBuffer iCmd);
	void EndFrame(VkCommandBuffer iCmd);

	void Resize(uint32_t iWidth, uint32_t iHeight);

	VulkanImage* GetColorTarget() const {return mColorBuffer.get(); }
	VulkanImage* GetDepthTarget() const { return mDepthBuffer.get(); }

	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }

	ImTextureID GetImguiTextureID() const { return mImGuiTextureID; }
	void BindToImgui();

private:
	void CreateColorBuffer();
	void CreateDepthBuffer();
	void SetupUI();

	ImTextureID mImGuiTextureID = 0;
	ImVec2 mRequestedSize{1280, 720};

	void CreateTextureSampler(VkDevice iDevice, VkPhysicalDevice iPhysicalDevice);
	VkSampler mSampler = VK_NULL_HANDLE;

	RenderContext* mContext = nullptr;
	std::unique_ptr<VulkanImage> mColorBuffer = nullptr;
	std::unique_ptr<VulkanImage> mDepthBuffer = nullptr;
	VkFormat mColorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	VkFormat mDepthFormat = VK_FORMAT_D24_UNORM_S8_UINT;
	uint32_t mWidth = 1280;
	uint32_t mHeight = 720;
};