#pragma once

#include <memory>
#include "graphics/RenderContext.h"

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
	std::unique_ptr<RenderContext> mContext = nullptr;
};
