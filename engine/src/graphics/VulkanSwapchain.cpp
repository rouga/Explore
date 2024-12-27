#include "VulkanSwapchain.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Window.h"
#include "VulkanInstance.h"

VulkanSwapchain::~VulkanSwapchain()
{
	PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = VK_NULL_HANDLE;
	vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(mInstance, "vkDestroySurfaceKHR");

	if (!vkDestroySurfaceKHR)
	{
		spdlog::error("Failed to find address of vkDestroySurfaceKHR.");
		exit(EXIT_FAILURE);
	}

	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	spdlog::error("GLFW Window Surface Destroyed.");
}

void VulkanSwapchain::Initialize(VkInstance iInstance, Window* iWindow)
{
	mInstance = iInstance;

	if(glfwCreateWindowSurface(iInstance, iWindow->GetGLFWWindow(), nullptr, &mSurface))
	{
		throw std::exception("Failed to create a GLFW Window surface.");
		exit(EXIT_FAILURE);
	}

	spdlog::info("GLFW Window surface created.");
}
