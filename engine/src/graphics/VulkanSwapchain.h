#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance;
class VulkanLogicalDevice;
class Window;

class VulkanSwapchain
{
public:
	VulkanSwapchain(){}
	~VulkanSwapchain();
	
	void Initialize(VkInstance iInstance, VulkanLogicalDevice* iLogicalDevice, Window* iWindow, uint32_t iNumSwapchainImages);

	uint32_t GetNumImages() const { return mNumSwapchainImages; }

private:
	void CreateSurface();
	void CreateSwapchain();

	uint32_t mNumSwapchainImages = 0;

	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
	VkInstance mInstance = VK_NULL_HANDLE;

	Window* mWindow = nullptr;
	VulkanLogicalDevice* mLogicalDevice = nullptr;

	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
};