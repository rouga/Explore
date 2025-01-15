#include "VulkanInstance.h"

#include <vector>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Utils.h"

VulkanInstance::VulkanInstance()
{
}

VulkanInstance::~VulkanInstance()
{
	vkDestroyInstance(mInstance, nullptr);
	spdlog::info("Vulkan Instance Destroyed.");
}

void VulkanInstance::Initialize(const char* iAppName)
{
	 std::vector<const char*> wLayers =
	 {
#if defined(_DEBUG)
		 "VK_LAYER_KHRONOS_validation"
#endif
	 };

	 std::vector<const char*> wExtensions =
	 {
		 VK_KHR_SURFACE_EXTENSION_NAME,
#if defined (_WIN32)
		 "VK_KHR_win32_surface",
#endif
#if defined (__APPLE__)
		 "VK_KHR_macos_surface",
#endif
#if defined (__LINUX__)
		 "VK_KHR_xcb_surface",
#endif
		 VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	 };

	 const VkApplicationInfo wAppInfo = 
	 {
		 .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		 .pNext = nullptr,
		 .pApplicationName = iAppName,
		 .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		 .pEngineName = "Explore",
		 .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		 .apiVersion = VK_API_VERSION_1_3
	 };

	 VkInstanceCreateInfo wCreateInfo = 
	 {
		 .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		 .pNext = nullptr,
		 .flags = 0,
		 .pApplicationInfo = &wAppInfo,
		 .enabledLayerCount = (uint32_t)wLayers.size(),
		 .ppEnabledLayerNames = wLayers.data(),
		 .enabledExtensionCount = (uint32_t)wExtensions.size(),
		 .ppEnabledExtensionNames = wExtensions.data(),
	 };

	 VkResult wResult = vkCreateInstance(&wCreateInfo, nullptr, &mInstance);
	 CHECK_VK_RESULT(wResult, "Instance Creation");
	 spdlog::info("Vulkan Instance Created.");
	 CmdDebug::SetupDebugUtils(mInstance);
}
