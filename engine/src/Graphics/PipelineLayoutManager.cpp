#include "PipelineLayoutManager.h"


#include "Core/Logger.h"

#include "Utils.h"

PipelineLayoutManager::PipelineLayoutManager(VkDevice iDevice)
	:mDevice(iDevice)
{}

PipelineLayoutManager::~PipelineLayoutManager()
{
	for (const auto& [wName, wLayout] : mLayouts) 
	{
		vkDestroyPipelineLayout(mDevice, wLayout, nullptr);
	}
}

void PipelineLayoutManager::CreateLayout(const std::vector<VkDescriptorSetLayout>& iDescriptorSetLayouts,
																				 const std::vector<VkPushConstantRange>& iPushConstantRanges,
																				 const std::string& iName)
{
	VkPipelineLayoutCreateInfo wLayoutInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	  .setLayoutCount = static_cast<uint32_t>(iDescriptorSetLayouts.size()),
		.pSetLayouts = iDescriptorSetLayouts.data(),
		.pushConstantRangeCount = static_cast<uint32_t>(iPushConstantRanges.size()),
		.pPushConstantRanges = iPushConstantRanges.data(),
	};

	VkPipelineLayout wLayout = VK_NULL_HANDLE;
	VkResult wResult = vkCreatePipelineLayout(mDevice, &wLayoutInfo, nullptr, &wLayout);
	CHECK_VK_RESULT(wResult, "Pipeline Layout Creation");
	mLayouts[iName] = wLayout;

	Logger::Get().mLogger->info("New Pipeline Layout ({0:s}) created with {1:d} Descriptor Set Layouts and {2:d} Push Constant Ranges",
					iName, iDescriptorSetLayouts.size(), iPushConstantRanges.size());
}

VkPipelineLayout PipelineLayoutManager::GetLayout(const std::string& name) const
{
	auto wIter = mLayouts.find(name);
	return (wIter != mLayouts.end()) ? wIter->second : nullptr;
}
