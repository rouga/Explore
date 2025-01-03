#pragma once

#include "graphics/RenderContext.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/DescriptorSetManager.h"
#include "graphics/PipelineLayoutManager.h"
#include "graphics/VulkanShader.h"
#include "graphics/VulkanGraphicsPipeline.h"

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
	std::unique_ptr<DescriptorSetManager> mDescriptorSetManager = nullptr;
	std::unique_ptr<PipelineLayoutManager> mPipelineLayoutManager = nullptr;
	std::unique_ptr<VulkanShader> mVS = nullptr;
	std::unique_ptr<VulkanShader> mFS = nullptr;
	std::unique_ptr<VulkanGraphicsPipeline> mPipeline = nullptr;

	Window* mWindow = nullptr;

private:
	void CreateDescriptorSetLayouts();
	void CreatePipelineLayouts();
	void CreateShaders();
	void CreatePipelines();
};