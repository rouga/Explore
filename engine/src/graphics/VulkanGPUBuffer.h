#pragma once

#include <vulkan/vulkan.h>

#include "VulkanMemoryPool.h"

class VulkanLogicalDevice;
class VulkanCommandBuffer;

class VulkanGPUBuffer
{
public:
	VulkanGPUBuffer(VkBufferUsageFlags iUsageFlags, VkMemoryPropertyFlags iMemPropertiesFlags);
	~VulkanGPUBuffer();

	void Initialize(VulkanLogicalDevice* iDevice, VkDeviceSize iSize, VulkanMemoryPool* iMemPool);

	// Upload to GPU Local Memory
	void Upload(VulkanCommandBuffer* iCmd, VulkanGPUBuffer* iStagingBuffer, VkDeviceSize iSize, VkDeviceSize iSrcOffset, VkDeviceSize iDstOffset);

	// Map memory to host (proper flags must be set)
	void* MapMemory(VkDeviceSize iOffset, VkMemoryMapFlags iFlags);
	void UnmapMemory();

	void FreeGPU();

	VkDevice mDevice = VK_NULL_HANDLE;
	VkBuffer mBuffer = VK_NULL_HANDLE;
	VulkanMemoryPool* mMemPool = nullptr;
	VulkanMemoryPool::MemoryBlock mMemBlock;
	VkMemoryPropertyFlags mMemProperties;
	VkBufferUsageFlags mUsageFlags = 0;
};