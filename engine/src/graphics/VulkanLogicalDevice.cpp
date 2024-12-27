#include "VulkanLogicalDevice.h"

#include <vector>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "VulkanPhysicalDevice.h"
#include "Utils.h"

VulkanLogicalDevice::VulkanLogicalDevice()
{
}

VulkanLogicalDevice::~VulkanLogicalDevice()
{
	vkDestroyDevice(mDevice, nullptr);
	spdlog::info("Logical Device Destoryed.");
}

void VulkanLogicalDevice::Initialize(VulkanPhysicalDevice* iPhysicalDevice)
{
	float wQueuePriorities[] = { 1.f };

	VkDeviceQueueCreateInfo wQueueCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = (uint32_t)iPhysicalDevice->GetQueueFamilyIndex(),
		.queueCount = 1,
		.pQueuePriorities = &wQueuePriorities[0]
	};

	std::vector<const char*> wDeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
	};

	VkDeviceCreateInfo wCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &wQueueCreateInfo,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = (uint32_t)wDeviceExtensions.size(),
		.ppEnabledExtensionNames = wDeviceExtensions.data(),
		.pEnabledFeatures = nullptr
	};

	VkResult wResult = vkCreateDevice(iPhysicalDevice->GetDevice(), &wCreateInfo, nullptr, &mDevice);
	CHECK_VK_RESULT(wResult, "Logical Device Creation");
	
	spdlog::info("Logical Device Created.");
}
