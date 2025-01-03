#pragma once

#include "graphics/RenderContext.h"

class Window;
class StaticMesh;

class Renderer
{
public:
	Renderer();

	void Initialize(Window* iWindow);
	void UploadMesh(StaticMesh* iMesh);
	void Render(StaticMesh* iMesh);

	std::unique_ptr<RenderContext> mContext = nullptr;
	Window* mWindow = nullptr;
};