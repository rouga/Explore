#include "Engine.h"

#include <iostream>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

Engine::Engine()
{
	mContext = std::make_unique<RenderContext>();
}

void Engine::Initialize()
{
	mContext->Initialize();
}

void Engine::Run()
{
	glfwPollEvents();
}
