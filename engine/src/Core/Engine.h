#pragma once

#include <memory>
#include "Renderer/Renderer.h"

#include "Scene/Model.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"

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
	void Update(float iDeltaTime);

	void OnResize(int iWidth, int iHeight);

	Model* GetModel() { return mModel.get(); }
	Camera* GetCamera() {return mCamera.get(); }
	Window* GetWindow() { return mWindow; }
	Light* GetLight() { return mDirLight.get(); }
	Renderer* GetRenderer() { return mRenderer.get(); }

private:
	// Private constructor for Singleton
	Engine() = default;
	std::unique_ptr<Model> mModel = nullptr;
	Window* mWindow = nullptr;

	std::unique_ptr<Renderer> mRenderer = nullptr;
	std::unique_ptr<Camera> mCamera = nullptr;
	std::unique_ptr<Light> mDirLight = nullptr;

	uint32_t mFrameNum = 0;

};
