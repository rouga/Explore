#include "VulkanDebugCallback.h"

#include <exception>
#include <iostream>

#include "VulkanInstance.h"
#include "Utils.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallbackfn(VkDebugUtilsMessageSeverityFlagBitsEXT iSeverity,
																											VkDebugUtilsMessageTypeFlagsEXT iType,
																											const VkDebugUtilsMessengerCallbackDataEXT* iCallbackData,
																											void* iUserData)
{
	printf("Debug Callback : %s\n", iCallbackData->pMessage);
	printf("\tSeverity : %s\n", GetDebugSeverityStr(iSeverity));
	printf("\tType : %s\n", GetDebugTypeStr(iType));
	printf("\tObjects :");

	for (uint32_t i=0; i < iCallbackData->objectCount; i++)
	{
		printf("%llx ", iCallbackData->pObjects[i].objectHandle);
	}

	return VK_FALSE;
}

VulkanDebugCallback::VulkanDebugCallback()
{
	
}

VulkanDebugCallback::~VulkanDebugCallback()
{
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = VK_NULL_HANDLE;
	vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT");

	if(!vkDestroyDebugUtilsMessenger)
	{
		std::cerr << ("Failed to find address of vkDestroyDebugUtilsMessenger.\n");
		exit(EXIT_FAILURE);
	}

	vkDestroyDebugUtilsMessenger(mInstance, mDebugMessenger, nullptr);

	printf("Debug Callback Destoryed.\n");
}

void VulkanDebugCallback::Initialize(VulkanInstance* iInstance)
{
	mInstance = iInstance->GetInstance();

	VkDebugUtilsMessengerCreateInfoEXT wMessengerCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
											 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		.pfnUserCallback = &DebugCallbackfn,
		.pUserData = nullptr
	};

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
	vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT");
	
	if(!vkCreateDebugUtilsMessenger)
	{
		throw std::exception("Failed to find address of vkCreateDebugUtilsMessenger");
	}

	VkResult wResult = vkCreateDebugUtilsMessenger(mInstance, &wMessengerCreateInfo, nullptr, &mDebugMessenger);
	CHECK_VK_RESULT(wResult, "Create Debug Utils Messenger");

	std::cout << "Debug Callback created." << std::endl;
}
