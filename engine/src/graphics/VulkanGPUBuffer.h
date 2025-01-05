#pragma once

#include <vulkan/vulkan.h>
#include "vma/vk_mem_alloc.h"

#include "VulkanMemoryPool.h"

class VulkanLogicalDevice;
class VulkanCommandBuffer;

class VulkanGPUBuffer
{
public:
	VulkanGPUBuffer(VkBufferUsageFlags iUsageFlags, VmaAllocationCreateFlags iAllocationFlags);
	~VulkanGPUBuffer();

	void Initialize(VulkanLogicalDevice* iDevice, VkDeviceSize iSize, VmaAllocator iAllocator);

	// Upload to GPU Local Memory
	void Upload(VulkanCommandBuffer* iCmd, VulkanGPUBuffer* iStagingBuffer, VkDeviceSize iSize, VkDeviceSize iSrcOffset, VkDeviceSize iDstOffset);

	// Map memory to host (proper flags must be set)
	void* MapMemory(VkDeviceSize iOffset, VkMemoryMapFlags iFlags);
	void UnmapMemory();

	void FreeGPU();

	VkDevice mDevice = VK_NULL_HANDLE;
	VkBuffer mBuffer = VK_NULL_HANDLE;
	VmaAllocation mAllocation;
	VmaAllocationInfo mAllocationInfo;
	VmaAllocator mAllocator;
	VmaAllocationCreateFlags mAllocationFlags;
	VkBufferUsageFlags mUsageFlags = 0;
};