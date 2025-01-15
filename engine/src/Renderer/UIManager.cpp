#include "UIManager.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include <Core/Window.h>
#include <Core/Input.h>

UIManager::UIManager(Window* iWindow)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;  // No rounding for viewport windows
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;  // Opaque background for platform windows
	}

	ImGui_ImplGlfw_InitForVulkan(iWindow->GetGLFWWindow(), true);

	spdlog::info("UIManager Created.");
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

	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

}

void UIManager::EndFrame()
{
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
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
