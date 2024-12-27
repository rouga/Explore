#include "Engine.h"

#include <iostream>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#include "Window.h"

Engine::Engine(Window* iWindow)
	:mWindow(iWindow)
{
	mContext = std::make_unique<RenderContext>();
}

void Engine::Initialize()
{
	mContext->Initialize(mWindow);
}

void Engine::Run()
{
	glfwPollEvents();
}
