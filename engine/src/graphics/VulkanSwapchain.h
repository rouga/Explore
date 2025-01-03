#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance;
class VulkanLogicalDevice;
class VulkanCommandBuffer;
class Window;

class VulkanSwapchain
{
public:
	VulkanSwapchain(){}
	~VulkanSwapchain();
	
	void Initialize(VkInstance iInstance, VulkanLogicalDevice* iLogicalDevice, Window* iWindow, uint32_t iNumSwapchainImages);

	void TransitionImageToPresent(VulkanCommandBuffer* iCmd, uint32_t iImageIndex);
	void TransitionImageToDraw(VulkanCommandBuffer* iCmd, uint32_t iImageIndex);

	uint32_t GetNumImages() const { return mNumSwapchainImages; }

	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;

	VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;

	const VkFormat mColorFormat = VK_FORMAT_B8G8R8A8_SRGB;

private:
	void CreateSurface();
	void CreateSwapchain();

	uint32_t mNumSwapchainImages = 0;

	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	VkInstance mInstance = VK_NULL_HANDLE;

	Window* mWindow = nullptr;
	VulkanLogicalDevice* mLogicalDevice = nullptr;
};