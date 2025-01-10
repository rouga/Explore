#pragma once

#include <vulkan/vulkan.h>

class VulkanDescriptorPool
{
public:
	struct PoolSizes 
	{
		uint32_t uniformBuffers = 100;
		uint32_t combinedImageSamplers = 100;
		uint32_t storageBuffers = 100;
		uint32_t storageImages = 100;
		uint32_t inputAttachments = 100;
	};

	VulkanDescriptorPool(VkDevice iDevice, uint32_t iMaxSets = 10, const PoolSizes& iPoolSizes = PoolSizes());
	~VulkanDescriptorPool();

	VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
	VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

	void Reset();

	VkDescriptorPool getHandle() const { return mPools; }
private:
	void CreatePool(const PoolSizes& iPoolSizes, uint32_t iMaxSets);

	VkDevice mDevice = VK_NULL_HANDLE;
	VkDescriptorPool mPools = VK_NULL_HANDLE;
};