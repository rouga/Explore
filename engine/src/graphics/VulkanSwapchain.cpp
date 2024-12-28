#include "VulkanSwapchain.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Window.h"

#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"
#include "Utils.h"

VulkanSwapchain::~VulkanSwapchain()
{
	for (size_t i = 0; i < mNumSwapchainImages; i++)
	{
		vkDestroyImageView(mLogicalDevice->mDevice, mImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(mLogicalDevice->mDevice, mSwapchain, nullptr);
	spdlog::info("Swapchain Destroyed.");

	PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = VK_NULL_HANDLE;
	vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(mInstance, "vkDestroySurfaceKHR");

	if (!vkDestroySurfaceKHR)
	{
		spdlog::error("Failed to find address of vkDestroySurfaceKHR.");
		exit(EXIT_FAILURE);
	}

	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	spdlog::info("GLFW Window Surface Destroyed.");
}

void VulkanSwapchain::Initialize(VkInstance iInstance, VulkanLogicalDevice* iLogicalDevice, Window* iWindow, uint32_t iNumSwapchainImages)
{
	mInstance = iInstance;
	mWindow = iWindow;
	mLogicalDevice = iLogicalDevice;
	mNumSwapchainImages = iNumSwapchainImages;

	CreateSurface();
	CreateSwapchain();
}

void VulkanSwapchain::CreateSurface()
{
	if (glfwCreateWindowSurface(mInstance, mWindow->GetGLFWWindow(), nullptr, &mSurface))
	{
		throw std::exception("Failed to create a GLFW Window surface.");
		exit(EXIT_FAILURE);
	}

	spdlog::info("GLFW Window surface created.");
}

void VulkanSwapchain::CreateSwapchain()
{
	VkSurfaceCapabilitiesKHR wSurfaceCapabilites;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mLogicalDevice->mPhysicalDevice->GetDevice(), mSurface, &wSurfaceCapabilites);

	uint32_t wQueueFamilyIndex = mLogicalDevice->mPhysicalDevice->GetQueueFamilyIndex();

	VkSwapchainCreateInfoKHR wSwapchainCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = mSurface,
		.minImageCount = mNumSwapchainImages,
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = wSurfaceCapabilites.currentExtent,
		.imageArrayLayers = 1,
		.imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &wQueueFamilyIndex,
		.preTransform = wSurfaceCapabilites.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_FIFO_KHR,
		.clipped = VK_TRUE
	};

	VkResult wResult = vkCreateSwapchainKHR(mLogicalDevice->mDevice, &wSwapchainCreateInfo, nullptr, &mSwapchain);
	CHECK_VK_RESULT(wResult, "Swapchain creation");

	mImages.resize(mNumSwapchainImages);
	mImageViews.resize(mNumSwapchainImages);

	wResult = vkGetSwapchainImagesKHR(mLogicalDevice->mDevice, mSwapchain, &mNumSwapchainImages, mImages.data());
	CHECK_VK_RESULT(wResult, "Swapchain Image Query");

	for(uint32_t i = 0; i < mNumSwapchainImages; i++)
	{
		mImageViews[i] = CreateImageView(mLogicalDevice->mDevice, mImages[i],
															VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1,1);
	}

	spdlog::info("Swapchain created with {:d} images", mNumSwapchainImages);
}
