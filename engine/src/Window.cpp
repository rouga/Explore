#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include <iostream>
#include <exception>

void KeyCallback(GLFWwindow* iWindow, int iKey, int iScancode, int iAction, int iMode)
{
	if(iKey == GLFW_KEY_ESCAPE && iAction == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(iWindow, GLFW_TRUE);
	}
}

Window::Window()
{
}

Window::~Window()
{
	glfwTerminate();
}

void Window::Initialize(int iWidth, int iHeight, std::string iTitle)
{
	if(!glfwInit())
	{
		throw std::exception("Failed to Initialize GLFW !");
	}

	if(!glfwVulkanSupported())
	{
		throw std::exception("Vulkan not supported on GLFW !");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	mWindow = glfwCreateWindow(iWidth, iHeight, iTitle.c_str(), nullptr, nullptr);

	if(!mWindow)
	{
		glfwTerminate();
		throw std::exception("Failed to initialize window !");
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(mWindow, KeyCallback);
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(mWindow);
}
