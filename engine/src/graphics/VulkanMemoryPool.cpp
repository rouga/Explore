#include "VulkanMemoryPool.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Utils.h"

VulkanMemoryPool::VulkanMemoryPool(VkDevice iDevice, VkPhysicalDevice iPhysicalDevice, VkDeviceSize iPoolSize, VkMemoryPropertyFlags iMemFlags)
	:mDevice(iDevice),
	mPhysicalDevice(iPhysicalDevice),
	mPoolSize(iPoolSize),
	mMemFlags(iMemFlags)
{
	// Query memory requirements and properties
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = mPoolSize;

	// Find a suitable memory type
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);
	allocInfo.memoryTypeIndex = FindMemoryType(mPhysicalDevice, iMemFlags);

	// Allocate the memory
	VkResult wResult = vkAllocateMemory(mDevice, &allocInfo, nullptr, &mMemoryPool);
	CHECK_VK_RESULT(wResult, "Memory Allocation");

	// Initialize a single free block
	mBlocks.push_back({ mMemoryPool, 0, mPoolSize, true });

	spdlog::info("Initialized Memory Pool with {:d} Mb", iPoolSize / 1024 / 1024);
}

VulkanMemoryPool::~VulkanMemoryPool()
{
	if (mMemoryPool != VK_NULL_HANDLE) 
	{
		vkFreeMemory(mDevice, mMemoryPool, nullptr);
		mMemoryPool = VK_NULL_HANDLE;
	}
}

VulkanMemoryPool::MemoryBlock VulkanMemoryPool::Allocate(VkDeviceSize iSize, VkDeviceSize iAlignment)
{
	for (MemoryBlock& wIterBlock : mBlocks) 
	{
		if (wIterBlock.free && wIterBlock.size >= iSize)
		{
			VkDeviceSize wAlignedOffset = (wIterBlock.offset + iAlignment - 1) & ~(iAlignment - 1);
			VkDeviceSize wPadding = wAlignedOffset - wIterBlock.offset;
			if (wIterBlock.size >= iSize + wPadding)
			{
				VkDeviceSize wRemainingSize = wIterBlock.size - iSize - wPadding;
				wIterBlock.free = false;
				wIterBlock.size = iSize;
				wIterBlock.offset = wAlignedOffset;
				if (wRemainingSize > 0)
				{
					mBlocks.push_back({ mMemoryPool, wAlignedOffset + iSize, wRemainingSize, true });
				}
				return MemoryBlock{ mMemoryPool , wAlignedOffset , iSize , false};
			}
		}
	}
	spdlog::error("No suitable memory block available.");
	exit(EXIT_FAILURE);
}

void VulkanMemoryPool::Free(const MemoryBlock& iBlock)
{
	for (auto& wBlock : mBlocks) 
	{
		if (wBlock.offset == iBlock.offset)
		{
			wBlock.free = true;
			break;
		}
	}

	// Coalesce adjacent free blocks
	for (size_t i = 0; i < mBlocks.size() - 1; ++i)
	{
		if (mBlocks[i].free && mBlocks[i + 1].free)
		{
			mBlocks[i].size += mBlocks[i + 1].size;
			mBlocks.erase(mBlocks.begin() + i + 1);
			--i;
		}
	}
}