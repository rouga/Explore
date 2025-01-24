#include "Engine.h"

#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Logger.h"

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

	mRenderer = std::make_unique<Renderer>();
	mRenderer->Initialize(mWindow);
	mCamera = std::make_unique<Camera>(mRenderer->mViewport.get());
	Light::DirectionalLightConfig wLightConfig;
	mDirLight = std::make_unique<Light>(wLightConfig);
	mModel = std::make_unique<Model>("resources/Helmet/DamagedHelmet.gltf");
	Logger::Get().mLogger->info("Number of meshes loaded to CPU : {:d}", mModel->GetNumMeshes());
	if(mModel)
	{
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

void Engine::Update(float iDeltaTime)
{
	Input::Get().Setup();
	glfwPollEvents();
	
	if(mWindow->IsMinimized())
	{
		return;
	}
	mDirLight->Update();
	mCamera->Update(iDeltaTime);
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
