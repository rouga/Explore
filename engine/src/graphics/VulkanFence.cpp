#include "VulkanFence.h"

#include "Utils.h"

VulkanFence::VulkanFence(VkDevice iDevice, VkFenceCreateFlags iFlags)
	:mDevice(iDevice)
{
	VkFenceCreateInfo wInfo{};
	wInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	wInfo.flags = iFlags;

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
