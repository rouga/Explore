#include "VulkanPhysicalDevice.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Utils.h"

VulkanPhysicalDevice::VulkanPhysicalDevice()
{

}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
}

void VulkanPhysicalDevice::Initialize(VkInstance iInstance)
{
	mInstance = iInstance;

	QueryDevice();
	LogSelectedDevice();
	QueryQueueFamily();
}

void VulkanPhysicalDevice::QueryDevice()
{
	uint32_t wNumDevices = 0;
	std::vector<VkPhysicalDevice> wDevices;

	VkResult wResult = vkEnumeratePhysicalDevices(mInstance, &wNumDevices, nullptr);
	CHECK_VK_RESULT(wResult, "Enumerate Physical Devices");

	spdlog::info("Number of Physical Devices : {:d}", wNumDevices);
	wDevices.resize(wNumDevices);

	wResult = vkEnumeratePhysicalDevices(mInstance, &wNumDevices, wDevices.data());
	CHECK_VK_RESULT(wResult, "Enumerate Physical Devices");

	// A Simple logic is implemented to selected the first Discrete GPU encountered.
	VkPhysicalDevice wPotentialDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties wPotentialDeviceProperties;
	for (uint32_t i = 0; i < wNumDevices; i++)
	{
		wPotentialDevice = wDevices[i];
		vkGetPhysicalDeviceProperties(wPotentialDevice, &wPotentialDeviceProperties);

		if (wPotentialDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			mDevice = wPotentialDevice;
			mDeviceProperties = wPotentialDeviceProperties;
		}

		break;
	}

	if (!wPotentialDevice)
	{
		spdlog::error("No suitable Physical Device was found.");
		exit(EXIT_FAILURE);
	}

	if (!mDevice)
	{
		mDevice = wPotentialDevice;
		mDeviceProperties = wPotentialDeviceProperties;
	}
}

void VulkanPhysicalDevice::QueryQueueFamily()
{
	uint32_t wNumQueueFamilies;
	std::vector<VkQueueFamilyProperties> wQueueFamilyProperties;

	vkGetPhysicalDeviceQueueFamilyProperties(mDevice, &wNumQueueFamilies, nullptr);
	wQueueFamilyProperties.resize(wNumQueueFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(mDevice, &wNumQueueFamilies, wQueueFamilyProperties.data());

	for (uint32_t i = 0; i < wQueueFamilyProperties.size(); i++)
	{
		if(wQueueFamilyProperties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
		{
			mQueueFamilyIndex = i;
			spdlog::info("A Queue Family supporting Graphics, Compute and Transfer was found (Queue Family {:d}).", i);
			break;
		}
	}

	if(mQueueFamilyIndex < 0)
	{
		spdlog::error("No Queue family supporting Graphics, Compute and Transfer was found.");
		exit(EXIT_FAILURE);
	}
}

void VulkanPhysicalDevice::LogSelectedDevice()
{
	spdlog::info("A Physical Device was Selected : \n");
	spdlog::info("\tDevice Name : {:s}", mDeviceProperties.deviceName);

	std::string wDeviceTypeStr;
	switch (mDeviceProperties.deviceType)
	{
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		wDeviceTypeStr = "Other";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		wDeviceTypeStr = "Integrated GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		wDeviceTypeStr = "Discrete GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		wDeviceTypeStr = "Virtual GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		wDeviceTypeStr = "CPU";
		break;
	}
	spdlog::info("\tDevice Type : {:s}", wDeviceTypeStr);

	spdlog::info("\tDevice Vendor ID : {:d}", mDeviceProperties.vendorID);
	spdlog::info("\tDevice Driver Version : {:d}", mDeviceProperties.driverVersion);

	uint32_t wAPIversion = mDeviceProperties.apiVersion;
	spdlog::info("\tDevice Vulkan API Version : {0:d}.{1:d}.{2:d}.{3:d}", 
		VK_API_VERSION_VARIANT(wAPIversion), VK_API_VERSION_MAJOR(wAPIversion), VK_API_VERSION_MINOR(wAPIversion), VK_API_VERSION_PATCH(wAPIversion));
}
