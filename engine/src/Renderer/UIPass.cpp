#include "UIPass.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Graphics/RenderContext.h"
#include "Renderer/Renderer.h"
#include "Core/Window.h"
#include "Core/Engine.h"

UIPass::UIPass(RenderContext* iContext)
	:RenderPass(iContext)
{
}

UIPass::~UIPass()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UIPass::Setup(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	mRenderPass = std::make_unique<VulkanRenderPass>(mContext->mLogicalDevice->mDevice);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForVulkan(mContext->mWindow->GetGLFWWindow(), true);

	VulkanDescriptorPool::PoolSizes wPoolSize =
	{
		.uniformBuffers = 1000,
		.combinedImageSamplers = 1000,
		.storageBuffers = 1000,
		.storageImages = 1000,
		.inputAttachments = 1000,
	};

	mPool = std::make_unique<VulkanDescriptorPool>(mContext->mLogicalDevice->mDevice, 1000, wPoolSize);
	
	VkPipelineRenderingCreateInfo wRenderingInfo =
	{
		 .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		 .pNext = nullptr,
		 .colorAttachmentCount = 1,
		 .pColorAttachmentFormats = &iFrameResources->mColorRenderTarget->GetFormat(),
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

}

void UIPass::Begin(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	uint32_t wCurrentImageIndex = mContext->mQueue->GetCurrentImageIndex();

	VkRenderingAttachmentInfo wColorAttachmentInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.pNext = nullptr,
		.imageView = iFrameResources->mColorRenderTarget->mImageView,
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
	};

	std::vector<VkRenderingAttachmentInfo> wColorAttachments {wColorAttachmentInfo};

	mRenderPass->Begin(iCmd, wColorAttachments,nullptr,
		VkExtent2D{ (uint32_t)Engine::Get().GetWindow()->GetWidth(), (uint32_t)Engine::Get().GetWindow()->GetHeight() });
}

void UIPass::Draw(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), iCmd);
}

void UIPass::End(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	mRenderPass->End(iCmd);
}
