#pragma once

#include <memory>
#include "Renderer.h"

#include "StaticMesh.h"
#include "Camera.h"

class Window;

class Engine 
{
public:
	Engine(Window* iWindow);
	~Engine();

	void Initialize();
	void Run();

private:
	StaticMesh mMesh;
	Window* mWindow = nullptr;

	std::unique_ptr<Renderer> mRenderer = nullptr;
	std::unique_ptr<OrbitCamera> mOrbitCamera = nullptr;

};
