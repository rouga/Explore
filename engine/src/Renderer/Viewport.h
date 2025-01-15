#pragma once

#include <imgui.h>
#include <vector>

#include "Graphics/VulkanImage.h"

class RenderContext;

class Viewport
{
public:
	Viewport(RenderContext* iContext);
	~Viewport();

	void Initialize();
	void ImguiSetup();
	void BeginFrame(VkCommandBuffer iCmd);
	void EndFrame(VkCommandBuffer iCmd);

	void Resize(int iWidth, int iHeight);

	VulkanImage* GetColorTarget(uint32_t iFrameIndex) const {return mColorBuffer[iFrameIndex].get(); }
	VulkanImage* GetDepthTarget(uint32_t iFrameIndex) const { return mDepthBuffer[iFrameIndex].get(); }

	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }

	void BindToImgui();

private:
	void CreateColorBuffer();
	void CreateDepthBuffer();
	void SetupUI();

	std::vector<ImTextureID> mImGuiTextureID;
	ImVec2 mRequestedSize{0, 0};

	void CreateTextureSampler(VkDevice iDevice, VkPhysicalDevice iPhysicalDevice);
	VkSampler mSampler = VK_NULL_HANDLE;

	RenderContext* mContext = nullptr;
	std::vector<std::unique_ptr<VulkanImage>> mColorBuffer;
	std::vector<std::unique_ptr<VulkanImage>> mDepthBuffer;
	VkFormat mColorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	VkFormat mDepthFormat = VK_FORMAT_D24_UNORM_S8_UINT;
	uint32_t mWidth = 1280;
	uint32_t mHeight = 720;
};