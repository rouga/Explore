#pragma once

#include "graphics/RenderContext.h"

class Window;

class Renderer
{
public:
	Renderer();

	void Initialize(Window* iWindow);

	void Render();

	std::unique_ptr<RenderContext> mContext = nullptr;
};