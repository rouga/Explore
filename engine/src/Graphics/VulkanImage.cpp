#include "VulkanImage.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "VulkanGPUBuffer.h"
#include "VulkanCommandBuffer.h"

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
	mExtent = iExtent;
	mImageConfig = iConfigInfo;
	if(iConfigInfo.WithMips)
	{
		mNumMipLevels = ComputeMipLevels(mExtent);
	}
	CreateImage(mExtent, mImageConfig.format, mImageConfig.usage, mImageConfig.tiling);
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
	wBarrier.subresourceRange.levelCount = mNumMipLevels;
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
	else if (iOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && iNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		wBarrier.srcAccessMask = 0;  // No access needed as the image is in undefined layout
		wBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		wSourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

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
	else if (iOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && iNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		wBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		wBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		wSourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	}
	else if (iOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && iNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	{
		wBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		wBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		wSourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	}
	else if (iOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && iNewLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		wBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		wBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		wSourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

	}
	else if (iOldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && iNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		wBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		wBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		wSourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		wDestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

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

void VulkanImage::UploadData(VulkanCommandBuffer* iCmd, VulkanGPUBuffer* iStagingBuffer, VkDeviceSize iSize, VkDeviceSize iStagingBufferOffset, VkExtent3D iDstExtent)
{
	// Upload original Texture
	VkBufferImageCopy wRegion{};
	wRegion.bufferOffset = iStagingBufferOffset;
	wRegion.bufferRowLength = 0;
	wRegion.bufferImageHeight = 0;
	wRegion.imageSubresource.aspectMask = mImageConfig.aspectFlags;
	wRegion.imageSubresource.mipLevel = 0;
	wRegion.imageSubresource.baseArrayLayer = 0;
	wRegion.imageSubresource.layerCount = 1;
	wRegion.imageOffset = { 0, 0, 0 };
	wRegion.imageExtent = iDstExtent;

	vkCmdCopyBufferToImage(iCmd->mCmd, iStagingBuffer->mBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &wRegion);

	// Generate Mips if necessary
	VkImageMemoryBarrier wBarrier{};
	wBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	wBarrier.image = mImage;
	wBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	wBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	wBarrier.subresourceRange.aspectMask = mImageConfig.aspectFlags;
	wBarrier.subresourceRange.baseArrayLayer = 0;
	wBarrier.subresourceRange.layerCount = 1;
	wBarrier.subresourceRange.levelCount = 1;

	uint32_t wMipWidth = mExtent.width;
	int32_t wMipHeight = mExtent.height;

	for (uint32_t i = 1; i < mNumMipLevels; i++) {
		// Transition previous mip level to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
		wBarrier.subresourceRange.baseMipLevel = i - 1;
		wBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		wBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		wBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		wBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(
			iCmd->mCmd,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &wBarrier
		);

		// Define the blit operation
		VkImageBlit wBlit{};
		wBlit.srcOffsets[0] = { 0, 0, 0 };
		wBlit.srcOffsets[1] = { (int)wMipWidth,(int)wMipHeight, 1 };
		wBlit.srcSubresource.aspectMask = mImageConfig.aspectFlags;
		wBlit.srcSubresource.mipLevel = i - 1;
		wBlit.srcSubresource.baseArrayLayer = 0;
		wBlit.srcSubresource.layerCount = 1;

		wBlit.dstOffsets[0] = { 0, 0, 0 };
		wBlit.dstOffsets[1] = { wMipWidth > 1 ? (int)wMipWidth / 2 : 1, wMipHeight > 1 ? (int)wMipHeight / 2 : 1, 1 };
		wBlit.dstSubresource.aspectMask = mImageConfig.aspectFlags;
		wBlit.dstSubresource.mipLevel = i;
		wBlit.dstSubresource.baseArrayLayer = 0;
		wBlit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(
			iCmd->mCmd,
			mImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &wBlit,
			VK_FILTER_LINEAR
		);

		// Transition current mip level to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
		wBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		wBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		wBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		wBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(
			iCmd->mCmd,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &wBarrier
		);

		// Update dimensions for next level
		wMipWidth = wMipWidth > 1 ? wMipWidth / 2 : 1;
		wMipHeight = wMipHeight > 1 ? wMipHeight / 2 : 1;
	}

	// Transition last level (or first level if no mipmaps were generated)
	wBarrier.subresourceRange.baseMipLevel = mNumMipLevels - 1;
	wBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	wBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	wBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	wBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(iCmd->mCmd,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &wBarrier);
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
		.mipLevels = mNumMipLevels,
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
			.levelCount = mNumMipLevels,
			.baseArrayLayer = 0,
			.layerCount = 1,
		}
	};

	VkResult wResult = vkCreateImageView(mDevice, &wViewInfo, nullptr, &mImageView);
	CHECK_VK_RESULT(wResult, "Image View Creation");
}

uint32_t VulkanImage::ComputeMipLevels(VkExtent3D iExtent)
{
	uint32_t wLargestDimension = std::max({ iExtent.width, iExtent.height, iExtent.depth });
	return static_cast<uint32_t>(std::floor(std::log2(wLargestDimension))) + 1;
}
