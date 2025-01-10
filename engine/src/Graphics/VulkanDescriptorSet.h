#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <memory>

class VulkanDescriptorSet 
{
public:

	VulkanDescriptorSet(VkDevice iDevice, VkDescriptorPool iPool, VkDescriptorSetLayout iLayout);

	VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
	VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = delete;

	void Update(uint32_t iBinding, VkDescriptorType iType, VkDescriptorBufferInfo* iBufferInfo);
	void Update(uint32_t iBinding, VkDescriptorImageInfo* iImageInfo);

	VkDescriptorSet GetHandle() const { return mDescriptorSet; }

private:
	VkDevice mDevice = VK_NULL_HANDLE;
	VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
};