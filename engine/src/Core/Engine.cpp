#include "Engine.h"

#include <iostream>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Window.h"
#include "Input.h"


Engine& Engine::Get()
{
	static Engine sInstance;
	return sInstance;
}

void Engine::Initialize(Window* iWindow)
{
	mWindow = iWindow;
	mRenderer = std::make_unique<Renderer>();
	mOrbitCamera = std::make_unique<OrbitCamera>(mWindow);

	mRenderer->Initialize(mWindow);
	Input::Get().Initialize(mWindow->GetGLFWWindow());

	mModel = std::make_unique<Model>("resources/cottage.obj");
	mRenderer->UploadGeometry(mModel.get());
	mModel->FreeCPU();
	mModel->GetTransform()->SetScale(glm::vec3{0.5, 0.5, 0.5});
	mModel->GetTransform()->SetRotation(glm::vec3{1.0, 0.0, 0.0}, 180);
}

void Engine::Shutdown()
{
	mRenderer->Flush();
	mModel->FreeGPU();
}

void Engine::Run()
{
	Input::Get().Setup();
	glfwPollEvents();
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
