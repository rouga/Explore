#pragma once

#include <glm/glm.hpp>

#include "graphics/RenderContext.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanShader.h"
#include "graphics/VulkanGraphicsPipeline.h"
#include "graphics/VulkanGPUBuffer.h"
#include "MainPass.h"

class Window;
class StaticMesh;
class OrbitCamera;

class Renderer
{
public:
	struct FrameUB
	{
		glm::mat4 ViewMatrix = glm::mat4();
		glm::mat4 ProjectionMatrix = glm::mat4();
	};

	Renderer();

	void Initialize(Window* iWindow);
	void UploadMesh(StaticMesh* iMesh);
	void Render();
	void Flush();
	void Resize(int iWidth, int iHeight);

	std::unique_ptr<RenderContext> mContext = nullptr;
	std::unique_ptr<MainPass> mMainPass = nullptr;

	Window* mWindow = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mFrameUB = nullptr;
};