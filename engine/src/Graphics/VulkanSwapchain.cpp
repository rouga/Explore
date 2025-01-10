#include "VulkanSwapchain.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Core/Window.h"

#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanCommandBuffer.h"
#include "Utils.h"

VulkanSwapchain::~VulkanSwapchain()
{
	DestroySwapchain();

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

void VulkanSwapchain::Resize(VkExtent2D iNewSize)
{
	DestroySwapchain();
	CreateSwapchain();
}

void VulkanSwapchain::TransitionImageToPresent(VulkanCommandBuffer* iCmd, uint32_t iImageIndex)
{
	VkImageSubresourceRange wSubresourceRange =
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	VkImageMemoryBarrier wToPresent =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mImages[iImageIndex],
		.subresourceRange = wSubresourceRange
	};

	vkCmdPipelineBarrier(iCmd->mCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &wToPresent);
}

void VulkanSwapchain::TransitionImageToDraw(VulkanCommandBuffer* iCmd, uint32_t iImageIndex)
{
	VkImageSubresourceRange wSubresourceRange =
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	
	VkImageMemoryBarrier wPresentToClear =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mImages[iImageIndex],
		.subresourceRange = wSubresourceRange
	};

	vkCmdPipelineBarrier(iCmd->mCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &wPresentToClear);
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
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mLogicalDevice->mPhysicalDevice->GetDevice(), mSurface, &mSurfaceCapabilites);

	uint32_t wQueueFamilyIndex = mLogicalDevice->mPhysicalDevice->GetQueueFamilyIndex();

	VkSwapchainCreateInfoKHR wSwapchainCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = mSurface,
		.minImageCount = mNumSwapchainImages,
		.imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = mSurfaceCapabilites.currentExtent,
		.imageArrayLayers = 1,
		.imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &wQueueFamilyIndex,
		.preTransform = mSurfaceCapabilites.currentTransform,
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
			VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1,1);
	}

	spdlog::info("Swapchain created with {:d} images", mNumSwapchainImages);
}

void VulkanSwapchain::DestroySwapchain()
{
	for (size_t i = 0; i < mNumSwapchainImages; i++)
	{
		vkDestroyImageView(mLogicalDevice->mDevice, mImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(mLogicalDevice->mDevice, mSwapchain, nullptr);
	spdlog::info("Swapchain Destroyed.");
}
