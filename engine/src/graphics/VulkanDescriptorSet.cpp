#include "VulkanDescriptorSet.h"

#include "Utils.h"

VulkanDescriptorSet::VulkanDescriptorSet(VkDevice iDevice, const std::vector<Binding>& iBindings)
	:mDevice(iDevice)
{
	CreateDescriptorSetLayout(iBindings);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	if (mDescriptorSetLayout != VK_NULL_HANDLE) 
	{
		vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);
	}
}

void VulkanDescriptorSet::Allocate(VkDescriptorPool iPool)
{
	VkDescriptorSetAllocateInfo wAllocInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = iPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &mDescriptorSetLayout,
	};

	VkResult wResult = vkAllocateDescriptorSets(mDevice, &wAllocInfo, &mDescriptorSet);
	CHECK_VK_RESULT(wResult, "Descriptor Set Allocation");
}

void VulkanDescriptorSet::Update(uint32_t iBinding, VkDescriptorType iType, VkDescriptorBufferInfo* iBufferInfo)
{
	VkWriteDescriptorSet wDescriptorWrite = 
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = mDescriptorSet,
		.dstBinding = iBinding,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = iType,
		.pBufferInfo = iBufferInfo
	};

	vkUpdateDescriptorSets(mDevice, 1, &wDescriptorWrite, 0, nullptr);
}

void VulkanDescriptorSet::Update(uint32_t iBinding, VkDescriptorImageInfo* iImageInfo)
{
	VkWriteDescriptorSet wDescriptorWrite =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = mDescriptorSet,
		.dstBinding = iBinding,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.pImageInfo = iImageInfo
	};

	vkUpdateDescriptorSets(mDevice, 1, &wDescriptorWrite, 0, nullptr);
}

void VulkanDescriptorSet::CreateDescriptorSetLayout(const std::vector<Binding>& iBindings)
{
	std::vector<VkDescriptorSetLayoutBinding> wLayoutBindings;
	wLayoutBindings.reserve(iBindings.size());

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
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = static_cast<uint32_t>(wLayoutBindings.size()),
		.pBindings = wLayoutBindings.data(),
	};

	VkResult wResult = vkCreateDescriptorSetLayout(mDevice, &layoutInfo, nullptr, &mDescriptorSetLayout);
	CHECK_VK_RESULT(wResult, "Descriptor Set Layout Creation");
}
