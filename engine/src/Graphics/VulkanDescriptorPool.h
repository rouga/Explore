#pragma once

#include <vulkan/vulkan.h>

class VulkanDescriptorPool
{
public:
	struct PoolSizes 
	{
		uint32_t uniformBuffers = 2500;
		uint32_t combinedImageSamplers = 2500;
		uint32_t storageBuffers = 2500;
		uint32_t storageImages = 2500;
		uint32_t inputAttachments = 2500;
	};

	VulkanDescriptorPool(VkDevice iDevice, uint32_t iMaxSets = 2500, const PoolSizes& iPoolSizes = PoolSizes());
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