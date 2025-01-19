#include "VulkanPhysicalDevice.h"


#include "Core/Logger.h"

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

	Logger::Get().mLogger->info("Number of Physical Devices : {:d}", wNumDevices);
	wDevices.resize(wNumDevices);

	wResult = vkEnumeratePhysicalDevices(mInstance, &wNumDevices, wDevices.data());
	CHECK_VK_RESULT(wResult, "Enumerate Physical Devices");

	// A Simple logic is implemented to selected the first Discrete GPU encountered.
	VkPhysicalDevice wPotentialDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties wPotentialDeviceProperties;
	VkPhysicalDeviceMemoryProperties wPotentialMemProperties;

	for (uint32_t i = 0; i < wNumDevices; i++)
	{
		wPotentialDevice = wDevices[i];
		vkGetPhysicalDeviceProperties(wPotentialDevice, &wPotentialDeviceProperties);
		vkGetPhysicalDeviceMemoryProperties(wPotentialDevice, &wPotentialMemProperties);

		if (wPotentialDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			mDevice = wPotentialDevice;
			mDeviceProperties = wPotentialDeviceProperties;
			mMemProperties = wPotentialMemProperties;
		}

		break;
	}

	if (!wPotentialDevice)
	{
		Logger::Get().mLogger->error("No Physical Device was found.");
		exit(EXIT_FAILURE);
	}

	if (!mDevice)
	{
		mDevice = wPotentialDevice;
		mDeviceProperties = wPotentialDeviceProperties;
		mMemProperties = wPotentialMemProperties;
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
			Logger::Get().mLogger->info("A Queue Family supporting Graphics, Compute and Transfer was found (Queue Family {:d}).", i);
			break;
		}
	}

	if(mQueueFamilyIndex < 0)
	{
		Logger::Get().mLogger->error("No Queue family supporting Graphics, Compute and Transfer was found.");
		exit(EXIT_FAILURE);
	}
}

void VulkanPhysicalDevice::LogSelectedDevice()
{
	Logger::Get().mLogger->info("A Physical Device was Selected : \n");
	Logger::Get().mLogger->info("\tDevice Name : {:s}", mDeviceProperties.deviceName);

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
	Logger::Get().mLogger->info("\tDevice Type : {:s}", wDeviceTypeStr);

	Logger::Get().mLogger->info("\tDevice Vendor ID : {:d}", mDeviceProperties.vendorID);
	Logger::Get().mLogger->info("\tDevice Driver Version : {:d}", mDeviceProperties.driverVersion);

	uint32_t wAPIversion = mDeviceProperties.apiVersion;
	Logger::Get().mLogger->info("\tDevice Vulkan API Version : {0:d}.{1:d}.{2:d}.{3:d}", 
		VK_API_VERSION_VARIANT(wAPIversion), VK_API_VERSION_MAJOR(wAPIversion), VK_API_VERSION_MINOR(wAPIversion), VK_API_VERSION_PATCH(wAPIversion));
}
