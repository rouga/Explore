#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanMemoryPool 
{
public:
	
	struct MemoryBlock {
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDeviceSize offset = 0;
		VkDeviceSize size = 0;
		bool free = false;
	};

	VulkanMemoryPool(VkDevice iDevice, VkPhysicalDevice iPhysicalDevice, VkDeviceSize iPoolSize, VkMemoryPropertyFlags iMemProps);
	~VulkanMemoryPool();

	MemoryBlock Allocate(VkDeviceSize iSize, VkDeviceSize iAlignment);
	void Free(const MemoryBlock& iBlock);

private:

	VkDevice mDevice = VK_NULL_HANDLE;
	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	VkDeviceMemory mMemoryPool = VK_NULL_HANDLE;
	VkDeviceSize mPoolSize = 0;
	VkMemoryPropertyFlags mMemFlags;
	std::vector<MemoryBlock> mBlocks;
};