#include "VulkanGPUBuffer.h"

#include "Utils.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanCommandBuffer.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

VulkanGPUBuffer::VulkanGPUBuffer(VkBufferUsageFlags iUsageFlags, VkMemoryPropertyFlags iMemPropertiesFlags)
	:mUsageFlags(iUsageFlags),
	mMemProperties(iMemPropertiesFlags)
{
}

VulkanGPUBuffer::~VulkanGPUBuffer()
{
	if(mBuffer)
	{
		FreeGPU();
	}
}

void VulkanGPUBuffer::Initialize(VulkanLogicalDevice* iDevice, VkDeviceSize iSize, VulkanMemoryPool* iMemPool)
{
	mMemPool = iMemPool;
	mDevice = iDevice->mDevice;

	VkBufferCreateInfo wCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = iSize,
		.usage = mUsageFlags,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	VkResult wResult = vkCreateBuffer(mDevice, &wCreateInfo, nullptr, &mBuffer);
	CHECK_VK_RESULT(wResult, "Buffer creation");

	// Create Memory for the buffer (TODO : Create one memory pool)
	VkMemoryRequirements wMemRequirements = {};
	vkGetBufferMemoryRequirements(mDevice, mBuffer, &wMemRequirements);

	mMemBlock = iMemPool->Allocate(iSize, wMemRequirements.alignment);

	// Bind Buffer to Memory
	wResult = vkBindBufferMemory(mDevice, mBuffer, mMemBlock.memory, mMemBlock.offset);
	CHECK_VK_RESULT(wResult, "Buffer Memory Bind");

}

void VulkanGPUBuffer::Upload(VulkanCommandBuffer* iCmd, VulkanGPUBuffer* iStagingBuffer, VkDeviceSize iSize, VkDeviceSize iSrcOffset, VkDeviceSize iDstOffset)
{
	assert((mMemProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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
		assert((mMemProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    void* wMappedMem = nullptr;
		VkResult wResult = vkMapMemory(mDevice, mMemBlock.memory, mMemBlock.offset, mMemBlock.size, iFlags, &wMappedMem);
		CHECK_VK_RESULT(wResult, "Memory mapping");
		return wMappedMem;
}

void VulkanGPUBuffer::UnmapMemory()
{
	vkUnmapMemory(mDevice, mMemBlock.memory);
}

void VulkanGPUBuffer::FreeGPU()
{
	vkDestroyBuffer(mDevice, mBuffer, nullptr);
	mBuffer = VK_NULL_HANDLE;
	mMemPool->Free(mMemBlock);
}
