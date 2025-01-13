#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <stdexcept>
#include <iostream>

class VulkanCommandBuffer;
class VulkanGPUBuffer;

class VulkanImage
{
public:

	struct ImageConfig
	{
		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
		VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
		VkMemoryPropertyFlags properties = 0;
		bool WithMips = false;
	};

	VulkanImage(VkDevice iDevice, VmaAllocator iAllocator);
	~VulkanImage();

	void Initialize(VkExtent3D iExtent, const ImageConfig& iConfigInfo);

	void Transition(VkCommandBuffer iCmd, VkImageLayout iOldLayout, VkImageLayout iNewLayout);

	// Upload data to GPU (with mips if enabled) and transition the layout to Shader Read Optimal
	void UploadData(VulkanCommandBuffer* iCmd, VulkanGPUBuffer* iStagingBuffer, VkDeviceSize iSize, VkDeviceSize iStagingBufferOffset, VkExtent3D iDstExtent);

	void Resize(VkExtent3D iExtent);

	void FreeGPU();

	const VkFormat& GetFormat() const { return mImageConfig.format; }

	const VkExtent3D& GetExtent() const { return mExtent; }

	VkImage mImage = VK_NULL_HANDLE;
	VkImageView mImageView = VK_NULL_HANDLE;
private:
	void CreateImage(VkExtent3D iExtent, VkFormat iFormat, VkImageUsageFlags iUsage,
		VkImageTiling iTiling);

	void CreateImageView(VkFormat iFormat, VkImageAspectFlags iAspectFlags);

	uint32_t ComputeMipLevels(VkExtent3D iExtent);

	VkExtent3D mExtent{0,0,0};
	VkDevice mDevice = VK_NULL_HANDLE;
	VmaAllocator mAllocator = VK_NULL_HANDLE;
	VmaAllocation mAllocation = VK_NULL_HANDLE;
	VmaAllocationInfo mAllocationInfo{};
	ImageConfig mImageConfig;
	uint32_t mNumMipLevels = 1; // Number of mips including the original texture

};