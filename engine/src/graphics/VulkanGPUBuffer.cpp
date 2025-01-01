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
		Destroy();
	}
}

void VulkanGPUBuffer::Initialize(VulkanLogicalDevice* iDevice, VkDeviceSize iSize)
{
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

	mAllocationSize = wMemRequirements.size;

	uint32_t wMemTypeIndex = GetMemoryTypeIndex(iDevice, wMemRequirements.memoryTypeBits, mMemProperties);

	VkMemoryAllocateInfo wMemInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = mAllocationSize,
		.memoryTypeIndex = wMemTypeIndex
	};

	wResult = vkAllocateMemory(mDevice, &wMemInfo, nullptr, &mMemory);
	CHECK_VK_RESULT(wResult, "Memory Allocation");

	// Bind Buffer to Memory
	wResult = vkBindBufferMemory(mDevice, mBuffer, mMemory, 0);
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
		VkResult wResult = vkMapMemory(mDevice, mMemory, iOffset, mAllocationSize, iFlags, &wMappedMem);
		CHECK_VK_RESULT(wResult, "Memory mapping");
		return wMappedMem;
}

void VulkanGPUBuffer::UnmapMemory()
{
	vkUnmapMemory(mDevice, mMemory);
}

void VulkanGPUBuffer::Destroy()
{
	vkDestroyBuffer(mDevice, mBuffer, nullptr);
	vkFreeMemory(mDevice, mMemory, nullptr);
	mBuffer = VK_NULL_HANDLE;
	mMemory = VK_NULL_HANDLE;
}

uint32_t VulkanGPUBuffer::GetMemoryTypeIndex(VulkanLogicalDevice* iDevice, uint32_t iMemTypeBitmask, VkMemoryPropertyFlags iMemPropertiesFlags)
{
	const VkPhysicalDeviceMemoryProperties& wPhysicalDeviceMemProperties = iDevice->mPhysicalDevice->GetMemProperties();

	for(uint32_t i = 0; i < wPhysicalDeviceMemProperties.memoryTypeCount; i++)
	{
		const VkMemoryType& wMemType = wPhysicalDeviceMemProperties.memoryTypes[i];
		uint32_t wCurrBitmask = (1 << i);
		bool wIsCurrMemTypeSupported = (iMemTypeBitmask & wCurrBitmask);
		bool wHasRequiredMemProps = ((wMemType.propertyFlags & iMemPropertiesFlags) == iMemPropertiesFlags);

		if(wIsCurrMemTypeSupported && wHasRequiredMemProps)
		{
			return i;
		}
	}

	spdlog::error("Cannot Find Memory Type");
	exit(EXIT_FAILURE);
}
