#include "VulkanImage.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Utils.h"

VulkanImage::VulkanImage(VkDevice iDevice, VmaAllocator iAllocator)
	:mDevice(iDevice),
	mAllocator(iAllocator)
{
}

VulkanImage::~VulkanImage()
{
	FreeGPU();
}

void VulkanImage::Initialize(VkExtent3D iExtent, const ImageConfig& iConfigInfo)
{
	mImageConfig = iConfigInfo;
	CreateImage(iExtent, mImageConfig.format, mImageConfig.usage, mImageConfig.tiling);
	CreateImageView(mImageConfig.format, mImageConfig.aspectFlags);
}

void VulkanImage::Transition(VkCommandBuffer iCmd, VkImageLayout iOldLayout, VkImageLayout iNewLayout)
{
	VkImageMemoryBarrier wBarrier{};
	wBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	wBarrier.oldLayout = iOldLayout;
	wBarrier.newLayout = iNewLayout;
	wBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	wBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	wBarrier.image = mImage;
	wBarrier.subresourceRange.aspectMask = mImageConfig.aspectFlags;
	wBarrier.subresourceRange.baseMipLevel = 0;
	wBarrier.subresourceRange.levelCount = 1;
	wBarrier.subresourceRange.baseArrayLayer = 0;
	wBarrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags wSourceStage;
	VkPipelineStageFlags wDestinationStage;

	if (iOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && iNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		wBarrier.srcAccessMask = 0;  // No access needed as the image is in undefined layout
		wBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		wSourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

	}
	else if (iOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && iNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		wBarrier.srcAccessMask = 0;
		wBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		wSourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (iOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && iNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		wBarrier.srcAccessMask = 0;
		wBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		wSourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (iOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && iNewLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		wBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		wBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		wSourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

	}
	else if (iOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && iNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	{
		wBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		wBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		wSourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	}
	else 
	{
		spdlog::error("Unsupported Image layout transition");
		exit(EXIT_FAILURE);
	}

	vkCmdPipelineBarrier(
		iCmd,
		wSourceStage, wDestinationStage,
		0,                      // No dependency flags
		0, nullptr,             // No memory barriers
		0, nullptr,             // No buffer memory barriers
		1, &wBarrier             // One image memory barrier
	);
}

void VulkanImage::Resize(VkExtent3D iExtent)
{
	FreeGPU();

	CreateImage(iExtent, mImageConfig.format, mImageConfig.usage, mImageConfig.tiling);
	CreateImageView(mImageConfig.format, mImageConfig.aspectFlags);
}

void VulkanImage::FreeGPU()
{
	if (mImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(mDevice, mImageView, nullptr);
		mImageView = VK_NULL_HANDLE;
	}

	if (mImage != VK_NULL_HANDLE)
	{
		vmaDestroyImage(mAllocator, mImage, mAllocation);
		mImage = VK_NULL_HANDLE;
	}
}

void VulkanImage::CreateImage(VkExtent3D iExtent, VkFormat iFormat, VkImageUsageFlags iUsage, VkImageTiling iTiling)
{
	VkImageCreateInfo wImageInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = iFormat,
		.extent = iExtent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = iTiling,
		.usage = iUsage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	VkResult wResult = vmaCreateImage(mAllocator, &wImageInfo, &allocInfo, &mImage, &mAllocation, &mAllocationInfo);
	CHECK_VK_RESULT(wResult, "Image Creation");

	spdlog::info("Image Create with dimensions {0:d}x{1:d} (Allocation Size : {2:d} MB)", iExtent.width, iExtent.height, mAllocationInfo.size / 1024 / 1024);
}

void VulkanImage::CreateImageView(VkFormat iFormat, VkImageAspectFlags iAspectFlags)
{
	VkImageViewCreateInfo wViewInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.image = mImage,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = iFormat,
		.subresourceRange = 
		{
			.aspectMask = iAspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		}
	};

	VkResult wResult = vkCreateImageView(mDevice, &wViewInfo, nullptr, &mImageView);
	CHECK_VK_RESULT(wResult, "Image View Creation");
}
