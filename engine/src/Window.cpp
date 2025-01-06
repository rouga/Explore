#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include <iostream>
#include <exception>

#include "Engine.h"

void KeyCallback(GLFWwindow* iWindow, int iKey, int iScancode, int iAction, int iMode)
{
	if(iKey == GLFW_KEY_ESCAPE && iAction == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(iWindow, GLFW_TRUE);
	}
}

void WindowResizeCallback(GLFWwindow* iWindow, int iWidth, int iHeight) {
	auto wEngine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(iWindow));
	wEngine->OnResize(iWidth, iHeight);
}

Window::Window(Engine* iEngine)
	:mEngine(iEngine)
{
}

Window::~Window()
{
	glfwTerminate();
}

void Window::Initialize(int iWidth, int iHeight, std::string iTitle)
{
	mWidth = iWidth;
	mHeight = iHeight;

	if(!glfwInit())
	{
		throw std::exception("Failed to Initialize GLFW !");
	}

	if(!glfwVulkanSupported())
	{
		throw std::exception("Vulkan not supported on GLFW !");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	mWindow = glfwCreateWindow(iWidth, iHeight, iTitle.c_str(), nullptr, nullptr);

	if(!mWindow)
	{
		glfwTerminate();
		throw std::exception("Failed to initialize window !");
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(mWindow, KeyCallback);
	glfwSetWindowUserPointer(mWindow, mEngine);
	glfwSetFramebufferSizeCallback(mWindow, WindowResizeCallback);
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(mWindow);
}

bool Window::IsMinimized() const
{
	return glfwGetWindowAttrib(mWindow, GLFW_ICONIFIED);
}
