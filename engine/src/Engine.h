#pragma once

#include <memory>
#include "Renderer.h"

class Window;

class Engine 
{
public:
	Engine(Window* iWindow);
	~Engine(){}

	void Initialize();
	void Run();

private:
	Window* mWindow = nullptr;
	std::unique_ptr<Renderer> mRenderer = nullptr;
};
