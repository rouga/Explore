#include "VulkanCommandBuffer.h"

#include "Utils.h"

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandPool iCmdPool, VkDevice iDevice)
{
	VkCommandBufferAllocateInfo wCmdAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = iCmdPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	VkResult wResult = vkAllocateCommandBuffers(iDevice, &wCmdAllocateInfo, &mCmd);
	CHECK_VK_RESULT(wResult, "Command Buffer Allocation");

	mCmdFinishSemaphore = std::make_unique<VulkanSemaphore>(iDevice);
}

void VulkanCommandBuffer::Begin(VkCommandBufferUsageFlags iFlags)
{
	VkCommandBufferBeginInfo wBeginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = iFlags,
		.pInheritanceInfo = nullptr
	};

	VkResult wResult = vkBeginCommandBuffer(mCmd, &wBeginInfo);
	CHECK_VK_RESULT(wResult, "Command Buffer Begin");
}

void VulkanCommandBuffer::End()
{
	VkResult wResult = vkEndCommandBuffer(mCmd);
	CHECK_VK_RESULT(wResult, "Command Buffer End");
}
