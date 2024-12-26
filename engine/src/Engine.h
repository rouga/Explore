#pragma once

#include <memory>
#include "graphics/RenderContext.h"

class Engine 
{
public:
	Engine();
	~Engine(){}

	void Initialize();
	void Run();

private:
	std::unique_ptr<RenderContext> mContext = nullptr;
};
