#include "UIPass.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Graphics/RenderContext.h"
#include "Renderer/Renderer.h"
#include "Core/Window.h"
#include "Core/Engine.h"


UIPass::UIPass(RenderContext* iContext)
	:RenderPass(iContext)
{
}

void UIPass::Setup(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	mRenderPass = std::make_unique<VulkanRenderPass>(mContext->mLogicalDevice->mDevice);

	VulkanDescriptorPool::PoolSizes wPoolSize =
	{
		.uniformBuffers = 1000,
		.combinedImageSamplers = 1000,
		.storageBuffers = 1000,
		.storageImages = 1000,
		.inputAttachments = 1000,
	};

	mPool = std::make_unique<VulkanDescriptorPool>(mContext->mLogicalDevice->mDevice, 10, wPoolSize);
	
	VkPipelineRenderingCreateInfo wRenderingInfo =
	{
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		 .pNext = nullptr,
		 .colorAttachmentCount = 1,
		 .pColorAttachmentFormats = &iFrameResources->mFrameRenderTarget->GetFormat(),
	};

	ImGui_ImplVulkan_InitInfo wImguiInitInfo = {};
	wImguiInitInfo.Instance = mContext->mInstance->GetInstance();
	wImguiInitInfo.PhysicalDevice = mContext->mPhysicalDevice->GetDevice();
	wImguiInitInfo.Device = mContext->mLogicalDevice->mDevice;
	wImguiInitInfo.Queue = mContext->mQueue->mQueue;
	wImguiInitInfo.DescriptorPool = mPool->getHandle(); 
	wImguiInitInfo.RenderPass = VK_NULL_HANDLE; //Dynamic Rendering
	wImguiInitInfo.MinImageCount = 2;
	wImguiInitInfo.ImageCount = mContext->mSwapchain->GetNumImages();
	wImguiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	wImguiInitInfo.UseDynamicRendering = VK_TRUE;
	wImguiInitInfo.PipelineRenderingCreateInfo = wRenderingInfo;

	ImGui_ImplVulkan_Init(&wImguiInitInfo);

	spdlog::info("UI pass setup completed.");
}

void UIPass::Begin(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	std::vector<VkImageView> wBackbuffer{ iFrameResources->mFrameRenderTarget->mImageView };

	mRenderPass->Begin(iCmd, wBackbuffer, nullptr,
		VkExtent2D{ (uint32_t)Engine::Get().GetWindow()->GetWidth(), (uint32_t)Engine::Get().GetWindow()->GetHeight() });
}

void UIPass::Draw(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), iCmd);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(nullptr, (void*)iCmd);
	}
}

void UIPass::End(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	mRenderPass->End(iCmd);
}
