#include "VulkanFence.h"

#include "Utils.h"

VulkanFence::VulkanFence(VkDevice iDevice)
	:mDevice(iDevice)
{
	VkFenceCreateInfo wInfo{};
	wInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	wInfo.flags = 0; // Can also use VK_FENCE_CREATE_SIGNALED_BIT for an initially signaled fence

	VkResult wResult = vkCreateFence(mDevice, &wInfo, nullptr, &mFence);
	CHECK_VK_RESULT(wResult, "Fence Creation");
}

VulkanFence::~VulkanFence()
{
	if(mFence)
	{
		vkDestroyFence(mDevice, mFence, nullptr);
	}
}

void VulkanFence::Reset()
{
	VkResult wResult = vkResetFences(mDevice, 1, &mFence);
	CHECK_VK_RESULT(wResult, "Fence Reset");
}

void VulkanFence::Wait()
{
	VkResult wResult = vkWaitForFences(mDevice, 1, &mFence, VK_TRUE, UINT64_MAX);
	CHECK_VK_RESULT(wResult, "Fence Wait");
}
