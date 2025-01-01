#pragma once

#include <vulkan/vulkan.h>

class VulkanLogicalDevice;
class VulkanCommandBuffer;

class VulkanGPUBuffer
{
public:
	VulkanGPUBuffer(VkBufferUsageFlags iUsageFlags, VkMemoryPropertyFlags iMemPropertiesFlags);
	~VulkanGPUBuffer();

	void Initialize(VulkanLogicalDevice* iDevice, VkDeviceSize iSize);

	// Upload to GPU Local Memory
	void Upload(VulkanCommandBuffer* iCmd, VulkanGPUBuffer* iStagingBuffer, VkDeviceSize iSize, VkDeviceSize iSrcOffset, VkDeviceSize iDstOffset);

	// Map memory to host (proper flags must be set)
	void* MapMemory(VkDeviceSize iOffset, VkMemoryMapFlags iFlags);
	void UnmapMemory();

	void Destroy();

	VkDevice mDevice = VK_NULL_HANDLE;
	VkBuffer mBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mMemory = VK_NULL_HANDLE;
	VkDeviceSize mAllocationSize = 0;

	VkMemoryPropertyFlags mMemProperties = 0;
	VkBufferUsageFlags mUsageFlags = 0;

private:
	uint32_t GetMemoryTypeIndex(VulkanLogicalDevice* iDeivce, uint32_t iMemTypeBitmask, VkMemoryPropertyFlags iMemPropertiesFlags);
};