#include "VulkanDescriptorPool.h"

#include <vector>


#include "Core/Logger.h"

#include "Utils.h"

VulkanDescriptorPool::VulkanDescriptorPool(VkDevice iDevice, uint32_t iMaxSets, const PoolSizes& iPoolSizes)
	:mDevice(iDevice)
{
	CreatePool(iPoolSizes, iMaxSets);
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	if (mPools != VK_NULL_HANDLE) 
	{
		vkDestroyDescriptorPool(mDevice, mPools, nullptr);
	}
}

void VulkanDescriptorPool::Reset()
{
	vkResetDescriptorPool(mDevice, mPools, 0);
}

void VulkanDescriptorPool::CreatePool(const PoolSizes& iPoolSizes, uint32_t iMaxSets)
{
	std::vector<VkDescriptorPoolSize> wPoolSizesList;

	if (iPoolSizes.uniformBuffers > 0) 
	{
		wPoolSizesList.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, iPoolSizes.uniformBuffers });
	}
	if (iPoolSizes.combinedImageSamplers > 0) 
	{
		wPoolSizesList.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, iPoolSizes.combinedImageSamplers });
	}
	if (iPoolSizes.storageBuffers > 0) 
	{
		wPoolSizesList.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, iPoolSizes.storageBuffers });
	}
	if (iPoolSizes.storageImages > 0) 
	{
		wPoolSizesList.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, iPoolSizes.storageImages });
	}
	if (iPoolSizes.inputAttachments > 0) 
	{
		wPoolSizesList.push_back({ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, iPoolSizes.inputAttachments });
	}

	VkDescriptorPoolCreateInfo wPoolInfo{};
	wPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	wPoolInfo.poolSizeCount = static_cast<uint32_t>(wPoolSizesList.size());
	wPoolInfo.pPoolSizes = wPoolSizesList.data();
	wPoolInfo.maxSets = iMaxSets;
	wPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	VkResult wResult = vkCreateDescriptorPool(mDevice, &wPoolInfo, nullptr, &mPools);
	CHECK_VK_RESULT(wResult, "Descriptor Pool Creation");

	Logger::Get().mLogger->info("Descriptor Pool Created");
}


