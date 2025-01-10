#include "VulkanGPUBuffer.h"

#include "Utils.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanCommandBuffer.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

VulkanGPUBuffer::VulkanGPUBuffer(VkBufferUsageFlags iUsageFlags, VmaAllocationCreateFlags iAllocationFlags)
	:mUsageFlags(iUsageFlags),
	mAllocationFlags(iAllocationFlags)
{
}

VulkanGPUBuffer::~VulkanGPUBuffer()
{
	if(mBuffer)
	{
		FreeGPU();
	}
}

void VulkanGPUBuffer::Initialize(VulkanLogicalDevice* iDevice, VkDeviceSize iSize, VmaAllocator iAllocator)
{
	mAllocator = iAllocator;
	mDevice = iDevice->mDevice;

	VkBufferCreateInfo wCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = iSize,
		.usage = mUsageFlags,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	VmaAllocationCreateInfo wVmaAllocInfo = {};
	wVmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	wVmaAllocInfo.flags = mAllocationFlags;

	VkResult wResult = vmaCreateBuffer(mAllocator, &wCreateInfo, &wVmaAllocInfo, &mBuffer, &mAllocation, &mAllocationInfo);

	CHECK_VK_RESULT(wResult, "Buffer Creation");
}

void VulkanGPUBuffer::Upload(VulkanCommandBuffer* iCmd, VulkanGPUBuffer* iStagingBuffer, VkDeviceSize iSize, VkDeviceSize iSrcOffset, VkDeviceSize iDstOffset)
{
	VkBufferCopy wCopy = 
	{
		.srcOffset = iSrcOffset,
		.dstOffset = iDstOffset,
		.size = iSize
	};
	vkCmdCopyBuffer(iCmd->mCmd, iStagingBuffer->mBuffer, mBuffer, 1, &wCopy);
}

void* VulkanGPUBuffer::MapMemory(VkDeviceSize iOffset, VkMemoryMapFlags iFlags)
{
	VmaAllocationCreateFlags wSequentialWriteCheck = mAllocationFlags | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	VmaAllocationCreateFlags wRandomAccessCheck = mAllocationFlags | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

	assert(wSequentialWriteCheck == VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				 || wRandomAccessCheck == VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

  void* wMappedMem = nullptr;
	VkResult wResult = vmaMapMemory(mAllocator, mAllocation, &wMappedMem);
	CHECK_VK_RESULT(wResult, "Memory mapping");

	return wMappedMem;
}

void VulkanGPUBuffer::UnmapMemory()
{
	vmaUnmapMemory(mAllocator, mAllocation);
}

void VulkanGPUBuffer::FreeGPU()
{
	vmaDestroyBuffer(mAllocator, mBuffer, mAllocation);
	mBuffer = VK_NULL_HANDLE;
	spdlog::info("Buffer destroyed with size : {:d}", mAllocationInfo.size);
}
