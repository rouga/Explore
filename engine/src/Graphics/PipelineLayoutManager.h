#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

class PipelineLayoutManager
{
public:
	PipelineLayoutManager(VkDevice iDevice);

	~PipelineLayoutManager();

	void CreateLayout( const std::vector<VkDescriptorSetLayout>& iDescriptorSetLayouts,
		const std::vector<VkPushConstantRange>& iPushConstantRanges,
		const std::string& iName);

	VkPipelineLayout GetLayout(const std::string& name) const;

private:
	VkDevice mDevice = VK_NULL_HANDLE;
	std::unordered_map<std::string, VkPipelineLayout> mLayouts;

};