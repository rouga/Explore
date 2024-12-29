#include "VulkanSemaphore.h"

#include "Utils.h"

VulkanSemaphore::VulkanSemaphore(VkDevice iDevice)
	:mDevice(iDevice)
{
	VkSemaphoreCreateInfo wCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};

	VkResult wResult = vkCreateSemaphore(iDevice, &wCreateInfo, nullptr, &mSemaphore);
	CHECK_VK_RESULT(wResult, "Semaphore Creation");
}

VulkanSemaphore::~VulkanSemaphore()
{
	vkDestroySemaphore(mDevice, mSemaphore, nullptr);
}
