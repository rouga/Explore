#include "Engine.h"

#include <iostream>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Window.h"
#include "Input.h"

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
	Input::Get().Initialize(mWindow->GetGLFWWindow());

	std::vector<Vertex> wVertices =
	{
		Vertex({-1.f, -1.f, 0.f}, {0.f, 0.f}),
		Vertex({1.f, 0.f, 0.f}, {0.f, 1.f}),
		Vertex({0.f, 1.f, 0.f}, {1.f, 1.f})
	};

	mMesh.SetName("Triangle");
	mMesh.Initialize(wVertices);

	mRenderer->UploadMesh(&mMesh);
	mMesh.FreeCPU();
}

void Engine::Run()
{
	Input::Get().Setup();
	glfwPollEvents();
	if(Input::Get().IsKeyDown(GLFW_KEY_R))
	{
		spdlog::info("R Key Pressed.");
	}
	mRenderer->Render(&mMesh);
}
