#include "UIManager.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include <imgui_internal.h>

#include <Core/Window.h>
#include <Core/Input.h>

UIManager::UIManager(Window* iWindow)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking

	io.Fonts->AddFontFromFileTTF("resources/fonts/DroidSans.ttf", 16);

	ImGui::StyleColorsLight();

	ImGui_ImplGlfw_InitForVulkan(iWindow->GetGLFWWindow(), true);

	spdlog::info("UIManager Created.");

	AddUIElement("Docking", []()
		{
			// Retrieve the main viewport (usually the entire window area)
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);

			// Create a window to host the dock space
			ImGuiWindowFlags dockspace_window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse  |
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoNavFocus;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::Begin("DockSpaceWindow", nullptr, dockspace_window_flags);
			ImGui::PopStyleVar(2);

			// Create the main dock space
			ImGuiID dockspace_id = ImGui::GetID("A");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
			static auto first_time = true;
			// Customize the layout using DockBuilder API
			if (first_time)
			{
				first_time = false;
				// Reset and split the dock space
				ImGui::DockBuilderRemoveNode(dockspace_id); // Clear any previous layout
				ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add a new dock node
				ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

				// Split the dock space into two horizontally
				ImGuiID leftDock, rightDock;
				ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, &leftDock, &rightDock);

				ImGuiID ViewportDock, LogDock;
				ImGui::DockBuilderSplitNode(rightDock, ImGuiDir_Up, 0.75f, &ViewportDock, &LogDock);

				ImGuiID SceneDock, PropertiesDock;
				ImGui::DockBuilderSplitNode(leftDock, ImGuiDir_Up, 0.6f, &SceneDock, &PropertiesDock);

				// Dock windows into the split spaces
				ImGui::DockBuilderDockWindow("Statistics", SceneDock);
				ImGui::DockBuilderDockWindow("Properties", PropertiesDock);
				ImGui::DockBuilderDockWindow("Viewport", ViewportDock);
				ImGui::DockBuilderDockWindow("Logger", LogDock);

				// Commit the dock layout
				ImGui::DockBuilderFinish(dockspace_id);
			}

			ImGui::End();
		});
}

UIManager::~UIManager()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	spdlog::info("UIManager Destroyed.");
}

void UIManager::BeginFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	Input::Get().UpdateCursorOnUI(io.WantCaptureMouse);

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UIManager::Execute()
{
	// Render registered UI elements
	for (const auto& [name, element] : mUIElements) 
	{
		element();
	}
}

void UIManager::AddUIElement(const std::string& iName, const UIElementCallback& iCallback)
{
	mUIElements[iName] = iCallback;
}

void UIManager::RemoveUIElement(const std::string& iName)
{
	mUIElements.erase(iName);
}

void UIManager::ClearUIElements()
{
	mUIElements.clear();
}
