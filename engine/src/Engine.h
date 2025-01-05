#pragma once

#include <memory>
#include "Renderer.h"

#include "StaticMesh.h"
#include "Camera.h"

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

	StaticMesh* GetMesh() { return &mMesh; }
	OrbitCamera* GetCamera() {return mOrbitCamera.get(); }

private:
	// Private constructor for Singleton
	Engine() = default;
	StaticMesh mMesh;
	Window* mWindow = nullptr;

	std::unique_ptr<Renderer> mRenderer = nullptr;
	std::unique_ptr<OrbitCamera> mOrbitCamera = nullptr;

};
