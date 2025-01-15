#pragma once

#include <memory>
#include "Renderer/Renderer.h"

#include "Scene/Model.h"
#include "Scene/Camera.h"
#include "Renderer/UIManager.h"

class Window;

class Engine 
{
public:
	// Get the singleton instance
	static Engine& Get();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void Initialize(Window* iWindow);
	void Shutdown();
	void Run();
	void OnResize(int iWidth, int iHeight);

	Model* GetModel() { return mModel.get(); }
	OrbitCamera* GetCamera() {return mOrbitCamera.get(); }
	Window* GetWindow() { return mWindow; }
	UIManager* GetUI() { return mUIManager.get(); }

private:
	// Private constructor for Singleton
	Engine() = default;
	std::unique_ptr<Model> mModel = nullptr;
	Window* mWindow = nullptr;

	std::unique_ptr<Renderer> mRenderer = nullptr;
	std::unique_ptr<UIManager> mUIManager = nullptr;
	std::unique_ptr<OrbitCamera> mOrbitCamera = nullptr;

};
