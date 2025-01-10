#include "DescriptorSetManager.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Utils.h"

DescriptorSetManager::DescriptorSetManager(VkDevice iDevice, uint32_t iNumImagesSwapchain)
	:mDevice(iDevice)
{
	mPools.resize(iNumImagesSwapchain);

	for (uint32_t i = 0; i < iNumImagesSwapchain; i++)
	{
		mPools[i] = std::make_unique<VulkanDescriptorPool>(iDevice);
	}
}

DescriptorSetManager::~DescriptorSetManager()
{
	for (const auto& [wName, wLayout] : mLayouts) 
	{
		vkDestroyDescriptorSetLayout(mDevice, wLayout, nullptr);
	}
}

void DescriptorSetManager::CreateLayout(const std::vector<Binding>& iBindings, const std::string& iName)
{
	std::vector<VkDescriptorSetLayoutBinding> wLayoutBindings;
	std::vector<VkDescriptorBindingFlags> wPartiallyBindingFlags;

	for (const auto& wBinding : iBindings)
	{
		VkDescriptorSetLayoutBinding wLayoutBinding =
		{
			.binding = wBinding.binding,
			.descriptorType = wBinding.type,
			.descriptorCount = wBinding.count,
			.stageFlags = wBinding.stageFlags,
			.pImmutableSamplers = (wBinding.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				? &wBinding.sampler
				: nullptr
		};

		wLayoutBindings.push_back(wLayoutBinding);
		wPartiallyBindingFlags.push_back(wBinding.partiallyBound ? VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT : 0);
	}


	VkDescriptorSetLayoutBindingFlagsCreateInfo wBindingFlags =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		.bindingCount = (uint32_t)wLayoutBindings.size(),
		.pBindingFlags = wPartiallyBindingFlags.data(),
	};

	VkDescriptorSetLayoutCreateInfo wLayoutInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = &wBindingFlags,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(wLayoutBindings.size()),
		.pBindings = wLayoutBindings.data(),
	};

	VkDescriptorSetLayout wLayout = VK_NULL_HANDLE;

	VkResult wResult = vkCreateDescriptorSetLayout(mDevice, &wLayoutInfo, nullptr, &wLayout);
	CHECK_VK_RESULT(wResult, "Descriptor Set Layout Creation");

	mLayouts[iName] = wLayout;

	spdlog::info("New Descriptor Set Layout ({0:s}) created with {1:d} bindings", iName, iBindings.size());
}

VkDescriptorSetLayout DescriptorSetManager::GetLayout(const std::string& iName) const
{
	auto wIter = mLayouts.find(iName);
	return (wIter != mLayouts.end()) ? wIter->second : nullptr;
}

VulkanDescriptorSet DescriptorSetManager::AllocateDescriptorSet(const std::string& iName, uint32_t iImageIndex)
{
	return VulkanDescriptorSet{ mDevice, mPools[iImageIndex]->getHandle(), GetLayout(iName) };
}

void DescriptorSetManager::ResetPool(uint32_t iImageIndex)
{
	mPools[iImageIndex]->Reset();
}
