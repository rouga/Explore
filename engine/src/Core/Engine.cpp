#include "Engine.h"

#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Window.h"
#include "Input.h"
#include "Renderer/TextureManager.h"


Engine& Engine::Get()
{
	static Engine sInstance;
	return sInstance;
}

void Engine::Initialize(Window* iWindow)
{
	mWindow = iWindow;
	Input::Get().Initialize(mWindow->GetGLFWWindow());
	mRenderer = std::make_unique<Renderer>();
	mOrbitCamera = std::make_unique<OrbitCamera>(mWindow);

	mRenderer->Initialize(mWindow);

	mModel = std::make_unique<Model>("resources/cottage.obj");
	spdlog::info("Number of meshes loaded to CPU : {:d}", mModel->GetNumMeshes());
	if(mModel)
	{
		mModel->GetTransform()->SetRotation(glm::vec3{1.0f, 0.0f, 0.0f}, 180);
		mRenderer->UploadGeometry(mModel.get());
		mModel->FreeCPU();
	}
}

void Engine::Shutdown()
{
	mRenderer->Flush();
	if(mModel)
	{
		mModel->FreeGPU();
	}
	TextureManager::Get().Shutdown();
}

void Engine::Run()
{
	Input::Get().Setup();
	glfwPollEvents();

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if(mWindow->IsMinimized())
	{
		return;
	}
	mOrbitCamera->Update();
	mRenderer->Render();
}

void Engine::OnResize(int iWidth, int iHeight)
{
	if(iWidth != 0 && iHeight != 0)
	{
		mWindow->SetWidth(iWidth);
		mWindow->SetHeight(iHeight);
		mRenderer->Resize(iWidth, iHeight);
	}
}
