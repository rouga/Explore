#include "Editor.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>
#include <algorithm>
#include <vector>

#include "Renderer/Renderer.h"
#include "Core/Input.h"
#include "Core/Timer.h"

#include "Scene/Entity.h"
#include "Core/Logger.h"

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
	Timer wTimer3;
	while (!mWindow->ShouldClose())
	{
		wTimer3.Start();
		wTimer.Start();
		Engine::Get().Update(mFrameTime / 1000.f);
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
		mFrameTime = wTimer3.Stop();
		mFrameTimes[mFrameNum % mFrameTimes.size()] = mFrameTime;
		mFrameNum += 1;
	}
}

void Editor::Shutdown()
{
	Logger::Get().mLogger->info("Closing Editor.");
	Engine::Get().Shutdown();
}

void Editor::SetupUI()
{
	mUIManager->AddUIElement("Button", [&]() {
		ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove);

		std::vector<Entity*> wEntites = { Engine::Get().GetCamera(), Engine::Get().GetLight() };

		bool selectedItem = false;
		static int wItemSelectedID = -1;
		for (int i = 0; i < wEntites.size() ; i++)
		{
			bool isSelected = (wItemSelectedID == i);
			if (ImGui::Selectable(wEntites[i]->GetName().c_str(), isSelected))
				wItemSelectedID = i;
		}
		ImGui::End();

		ImGui::Begin("Statistics", nullptr, ImGuiWindowFlags_NoMove);
		ImGui::Text("Frame Time : %.3f ms | %d FPS", mFrameTime, (int)(1000.f / mFrameTime));
		ImGui::PlotLines("##", mFrameTimes.data(), mFrameTimes.size(), 0, nullptr, 0.0f, 20.0f, ImVec2(0, 50));
		ImGui::Text("CPU Time : %.3f ms", mCPUFrameTime);
		ImGui::Text("Engine Update CPU Time : %.3f ms", mEngineCPU);
		ImGui::Text("Renderer Update CPU Time : %.3f ms", mRenderCPU);
		ImGui::End();

		ImGui::Begin("Detail", nullptr, ImGuiWindowFlags_NoMove);
		if(wItemSelectedID >= 0)
		{
			ImGui::Text(" === %s Properties ===", wEntites[wItemSelectedID]->GetName().c_str());
			mUIManager->DrawUIProperties(wEntites[wItemSelectedID]);
		}
		ImGui::End();

		ImGui::Begin("Logger", nullptr, ImGuiWindowFlags_NoMove);
		const auto& wLogMsgs = Logger::Get().mImguiSink->get_messages();
		for (const auto& wMsg : wLogMsgs)
		{
			ImGui::TextUnformatted(wMsg.c_str());
		}
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

