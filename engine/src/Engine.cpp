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

Engine::~Engine()
{
	mRenderer->Flush();
	mMesh.FreeGPU();
}

void Engine::Initialize()
{
	mRenderer->Initialize(mWindow);

	std::vector<Vertex> wVertices =
	{
		Vertex({-1.f, -1.f, 0.f}, {0.f, 0.f}),
		Vertex({0.f, -1.f, 0.f}, {0.f, 1.f}),
		Vertex({0.f, 1.f, 0.f}, {1.f, 1.f})
	};

	mMesh.SetName("Triangle");
	mMesh.Initialize(wVertices);

	mRenderer->UploadMesh(&mMesh);
}

void Engine::Run()
{
	glfwPollEvents();
	mRenderer->Render(&mMesh);
}
