#pragma once

#include <glm/glm.hpp>

#include "graphics/RenderContext.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanShader.h"
#include "graphics/VulkanGraphicsPipeline.h"
#include "graphics/VulkanGPUBuffer.h"

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

	std::unique_ptr<RenderContext> mContext = nullptr;
	std::unique_ptr<VulkanRenderPass> mMainPass = nullptr;
	std::unique_ptr<VulkanShader> mVS = nullptr;
	std::unique_ptr<VulkanShader> mFS = nullptr;
	std::unique_ptr<VulkanGraphicsPipeline> mPipeline = nullptr;

	Window* mWindow = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mFrameUB = nullptr;

private:
	void CreateDescriptorSetLayouts();
	void CreatePipelineLayouts();
	void CreateShaders();
	void CreatePipelines();
};