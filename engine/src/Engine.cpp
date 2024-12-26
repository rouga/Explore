#include "Engine.h"

#include <iostream>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

void Engine::Initialize() 
{
	mInstance = std::make_unique<VulkanInstance>();
	mInstance->Initialize("Editor");
}

void Engine::Run()
{
	glfwPollEvents();
}
