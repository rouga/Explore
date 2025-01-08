#pragma once

#include <glm/glm.hpp>

#include "Graphics/RenderContext.h"
#include "Graphics/VulkanRenderPass.h"
#include "Graphics/VulkanShader.h"
#include "Graphics/VulkanGraphicsPipeline.h"
#include "Graphics/VulkanGPUBuffer.h"
#include "MainPass.h"

class Window;
class StaticMesh;
class OrbitCamera;
class Model;

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
	void UploadModel(Model* iModel);
	void Render();
	void Flush();
	void Resize(int iWidth, int iHeight);

	std::unique_ptr<RenderContext> mContext = nullptr;
	std::unique_ptr<MainPass> mMainPass = nullptr;

	Window* mWindow = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mFrameUB = nullptr;
};