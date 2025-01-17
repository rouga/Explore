#include "Editor.h"

#define FMT_UNICODE 0
#include "spdlog/spdlog.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

#include "Renderer/Renderer.h"
#include "Core/Input.h"
#include "Core/Timer.h"

Editor::Editor()
{
}

void Editor::Initialize()
{
	spdlog::info("Launching Editor...");
	
	mWindow = std::make_unique<Window>(&Engine::Get());
	mWindow->Initialize(1920, 1080, "Explore Editor");
	Input::Get().Initialize(mWindow->GetGLFWWindow());
	mUIManager = std::make_unique<UIManager>(mWindow.get());
	SetupUI();
	
	Engine::Get().Initialize(mWindow.get());

	mRenderer = Engine::Get().GetRenderer();
	mRenderer->mViewport->ImguiSetup();
}

void Editor::Run()
{
	Timer wTimer;
	Timer wTimer2;
	while (!mWindow->ShouldClose())
	{
		wTimer.Start();
		Engine::Get().Update();
		mEngineCPU = wTimer.Peek();
		wTimer.Pause();
		mRenderer->StartFrame();
		wTimer.Resume();
		wTimer2.Start();
		mRenderer->RenderScene();
		mUIManager->BeginFrame();
		mRenderer->FinishFrame(mUIManager.get());
		mRenderer->Present();
		mRenderCPU = wTimer2.Stop();
		mCPUFrameTime = wTimer.Stop();
	}
}

void Editor::Shutdown()
{
	spdlog::info("Closing Editor.");
	Engine::Get().Shutdown();
}

void Editor::SetupUI()
{
	mUIManager->AddUIElement("Button", [&]() {
		ImGui::Begin("Statistics", nullptr, ImGuiWindowFlags_NoMove);
		ImGui::Text("CPU Time : %.3f ms", mCPUFrameTime);
		ImGui::Text("Engine Update CPU Time : %.3f ms", mEngineCPU);
		ImGui::Text("Renderer Update CPU Time : %.3f ms", mRenderCPU);
		ImGui::End();

		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoMove);
		ImGui::Text("Hello, World!");
		ImGui::End();

		ImGui::Begin("Logger", nullptr, ImGuiWindowFlags_NoMove);
		ImGui::Text("Hello, World!");
		ImGui::End();
		});

	mUIManager->AddUIElement("Viewport", [&]() {

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove);

		bool isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
		if (isHovered)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.WantCaptureMouse = false;
		}

		ImVec2 wRequestedSize = ImGui::GetContentRegionAvail();

		Viewport* wViewport = mRenderer->mViewport.get();

		if (wRequestedSize.x > 0 && wRequestedSize.y > 0)
		{
			wViewport->SetWidth((uint32_t)wRequestedSize.x);
			wViewport->SetHeight((uint32_t)wRequestedSize.y);
		}

		ImGui::Image(wViewport->GetImGuiTextureID(),
			ImVec2{ (float)wViewport->GetColorTarget()->GetExtent().width,
											 (float)wViewport->GetColorTarget()->GetExtent().height });

		ImGui::End(); }
	);
}

