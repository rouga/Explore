#pragma once

#include <glm/glm.hpp>

#include "Graphics/RenderContext.h"
#include "Graphics/VulkanRenderPass.h"
#include "Graphics/VulkanShader.h"
#include "Graphics/VulkanGraphicsPipeline.h"
#include "Graphics/VulkanGPUBuffer.h"
#include "MainPass.h"
#include "UIPass.h"
#include "Viewport.h"

class Window;
class StaticMesh;
class OrbitCamera;
class Model;
class UIManager;

static constexpr uint32_t sMaxNumberMeshes = 2500;
static constexpr uint32_t sMaxNumberLights = 8;

struct LightUB
{
	alignas(16) glm::vec3 Color;
	alignas(16) glm::vec3 Position;
	alignas(16) glm::vec3 Direction;
	float Intensity;
	int isDirectional = false;
	int IsPoint = false;
	int isSpot = false;
};

struct alignas(64) FrameUB
{
	glm::mat4 ViewMatrix = glm::mat4();
	glm::mat4 ProjectionMatrix = glm::mat4();
	alignas(64) LightUB Light[sMaxNumberLights];
	int LightsCount = 0;
};

struct alignas(64) ObjectUB
{
	glm::mat4 ModelMatrix = glm::mat4();
	glm::mat4 NormalMatrix = glm::mat4();
	int HasUV = false;
};

struct FrameResources
{
	FrameUB FrameUB;
	VulkanGPUBuffer* mFrameUniformBuffer = nullptr;
	VulkanGPUBuffer* mObjectsUniformBuffer = nullptr;
	VulkanImage* mFrameRenderTarget = nullptr;
	Viewport* mViewport = nullptr;
};

class Renderer
{
public:
	Renderer();

	void Initialize(Window* iWindow);
	void UploadGeometry(Model* iModel);
	void Flush();

	void Resize(int iWidth, int iHeight);

	void StartFrame();
	void RenderScene();
	void FinishFrame(UIManager* iUIManager);
	void Present();

	std::unique_ptr<RenderContext> mContext = nullptr;
	std::unique_ptr<MainPass> mMainPass = nullptr;
	std::unique_ptr<UIPass> mUIPass = nullptr;

	Window* mWindow = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mFrameUB = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mObjectsUB = nullptr;
	std::unique_ptr<Viewport> mViewport = nullptr;
	std::unique_ptr<VulkanImage> mFrameRenderTarget = nullptr;

private:
	FrameUB UpdateFrameUB();
	void UpdateObjectsUB();

	uint32_t mCurrentFrameInFlight = 0;
	uint32_t mCurrentSwapchainImageIndex = 0;
	uint32_t mFrameNum = 0;
};