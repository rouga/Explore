#include "Engine.h"

#include <iostream>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#include "Window.h"

Engine::Engine(Window* iWindow)
	:mWindow(iWindow)
{
	mRenderer = std::make_unique<Renderer>();
}

void Engine::Initialize()
{
	mRenderer->Initialize(mWindow);
}

void Engine::Run()
{
	glfwPollEvents();
	mRenderer->Render();
}
