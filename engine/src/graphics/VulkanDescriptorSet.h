#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <memory>

class VulkanDescriptorSet 
{
public:
	struct Binding 
	{
		uint32_t binding;
		VkDescriptorType type;
		uint32_t count;
		VkShaderStageFlags stageFlags;
		VkSampler sampler; // Optional, for sampled images
	};

	VulkanDescriptorSet(VkDevice iDevice, const std::vector<Binding>& iBindings);
	~VulkanDescriptorSet();

	VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
	VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = delete;

	void Allocate(VkDescriptorPool iPool);
	void Update(uint32_t iBinding, VkDescriptorType iType, VkDescriptorBufferInfo* iBufferInfo);
	void Update(uint32_t iBinding, VkDescriptorImageInfo* iImageInfo);

	VkDescriptorSet GetHandle() const { return mDescriptorSet; }
	VkDescriptorSetLayout GetLayout() const { return mDescriptorSetLayout; }

private:
	void CreateDescriptorSetLayout(const std::vector<Binding>& iBindings);

	VkDevice mDevice = VK_NULL_HANDLE;
	VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
	std::vector<VkWriteDescriptorSet> mWriteDescriptorSets;
};