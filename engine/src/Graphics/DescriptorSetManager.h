#pragma once

#include <string>

#include <vulkan/vulkan.h>

#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"

class DescriptorSetManager
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

	DescriptorSetManager(VkDevice iDevice, uint32_t iNumImagesSwapchain);
	~DescriptorSetManager();

	void CreateLayout(const std::vector<Binding>& iBindings, const std::string& iName);

	VkDescriptorSetLayout GetLayout(const std::string& iName) const;
	VulkanDescriptorSet AllocateDescriptorSet(const std::string& iName, uint32_t iImageIndex);
	void ResetPool(uint32_t iImageIndex);

private:
	VkDevice mDevice;
	std::vector<std::unique_ptr<VulkanDescriptorPool>> mPools;
	std::unordered_map<std::string, VkDescriptorSetLayout> mLayouts;
};