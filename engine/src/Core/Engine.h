#pragma once

#include <memory>
#include "Renderer/Renderer.h"

#include "Scene/Model.h"
#include "Scene/Camera.h"

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
	void Update();

	void OnResize(int iWidth, int iHeight);

	Model* GetModel() { return mModel.get(); }
	OrbitCamera* GetCamera() {return mOrbitCamera.get(); }
	Window* GetWindow() { return mWindow; }
	Renderer* GetRenderer() { return mRenderer.get(); }

	float mFrameTime = 0;

private:
	// Private constructor for Singleton
	Engine() = default;
	std::unique_ptr<Model> mModel = nullptr;
	Window* mWindow = nullptr;

	std::unique_ptr<Renderer> mRenderer = nullptr;
	std::unique_ptr<OrbitCamera> mOrbitCamera = nullptr;

};
