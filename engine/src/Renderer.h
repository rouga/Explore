#pragma once

#include "graphics/RenderContext.h"
#include "graphics/VulkanRenderPass.h"

class Window;
class StaticMesh;

class Renderer
{
public:
	Renderer();

	void Initialize(Window* iWindow);
	void UploadMesh(StaticMesh* iMesh);
	void Render(StaticMesh* iMesh);
	void Flush();

	std::unique_ptr<RenderContext> mContext = nullptr;
	std::unique_ptr<VulkanRenderPass> mMainPass = nullptr;
	Window* mWindow = nullptr;
};