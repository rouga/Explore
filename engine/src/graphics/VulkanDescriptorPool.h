#pragma once

#include <vulkan/vulkan.h>

class VulkanDescriptorPool
{
public:
	struct PoolSizes 
	{
		uint32_t uniformBuffers = 10;
		uint32_t combinedImageSamplers = 10;
		uint32_t storageBuffers = 10;
		uint32_t storageImages = 10;
		uint32_t inputAttachments = 10;
	};

	VulkanDescriptorPool(VkDevice iDevice, uint32_t iMaxSets = 1, const PoolSizes& iPoolSizes = PoolSizes());
	~VulkanDescriptorPool();

	VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
	VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

	VkDescriptorPool getHandle() const { return mPool; }
private:
	void CreatePool(const PoolSizes& iPoolSizes, uint32_t iMaxSets);

	VkDevice mDevice = VK_NULL_HANDLE;
	VkDescriptorPool mPool = VK_NULL_HANDLE;
};