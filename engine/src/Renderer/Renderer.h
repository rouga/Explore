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

struct FrameUB
{
	glm::mat4 ViewMatrix = glm::mat4();
	glm::mat4 ProjectionMatrix = glm::mat4();
};

struct alignas(64) ObjectUB
{
	glm::mat4 ModelMatrix = glm::mat4();
	int HasUV = false;
};

struct FrameResources
{
	FrameUB FrameUB;
	VulkanGPUBuffer* mFrameUniformBuffer = nullptr;
	VulkanGPUBuffer* mObjectsUniformBuffer = nullptr;
	VulkanImage* mColorRenderTarget = nullptr;
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Initialize(Window* iWindow);
	void UploadGeometry(Model* iModel);
	void Render();
	void Flush();
	void Resize(int iWidth, int iHeight);

	std::unique_ptr<RenderContext> mContext = nullptr;
	std::unique_ptr<MainPass> mMainPass = nullptr;

	Window* mWindow = nullptr;
	std::unique_ptr<VulkanImage> mColorRenderTarget = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mFrameUB = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mObjectsUB = nullptr;

private:
	void FillUniformBuffer();
	static constexpr uint32_t mMaxNumberMeshes = 2000;

};