#include "VulkanDescriptorSet.h"

#include "Utils.h"

VulkanDescriptorSet::VulkanDescriptorSet(VkDevice iDevice, VkDescriptorPool iPool,  VkDescriptorSetLayout iLayout)
	:mDevice(iDevice)
{

	VkDescriptorSetAllocateInfo wAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = iPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &iLayout,
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