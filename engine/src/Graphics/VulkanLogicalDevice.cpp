#include "VulkanLogicalDevice.h"

#include <vector>


#include "Core/Logger.h"

#include "VulkanPhysicalDevice.h"
#include "Utils.h"

VulkanLogicalDevice::VulkanLogicalDevice()
{
}

VulkanLogicalDevice::~VulkanLogicalDevice()
{
	vkDestroyDevice(mDevice, nullptr);
	Logger::Get().mLogger->info("Logical Device Destoryed.");
}

void VulkanLogicalDevice::Initialize(VulkanPhysicalDevice* iPhysicalDevice)
{
	mPhysicalDevice = iPhysicalDevice;

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
		"VK_KHR_maintenance1",
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	};

	auto Core_1_2 = VkPhysicalDeviceVulkan12Features
	{
	.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
	.pNext = nullptr,
	.descriptorBindingPartiallyBound = VK_TRUE,
	.bufferDeviceAddress = VK_TRUE,
	};

	auto Core_1_3 = VkPhysicalDeviceVulkan13Features
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.pNext = &Core_1_2,
		.dynamicRendering = VK_TRUE,
	};

	VkDeviceCreateInfo wCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &Core_1_3,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &wQueueCreateInfo,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = (uint32_t)wDeviceExtensions.size(),
		.ppEnabledExtensionNames = wDeviceExtensions.data(),
	};

	VkResult wResult = vkCreateDevice(iPhysicalDevice->GetDevice(), &wCreateInfo, nullptr, &mDevice);
	CHECK_VK_RESULT(wResult, "Logical Device Creation");
	
	Logger::Get().mLogger->info("Logical Device Created.");
}
