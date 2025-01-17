#include "Viewport.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>
#include <backends/imgui_impl_vulkan.h>

#include "Graphics/RenderContext.h"
#include "Graphics/Utils.h"

#include "Core/Engine.h"

Viewport::Viewport(RenderContext* iContext)
	:mContext(iContext)
{
}

Viewport::~Viewport()
{
	vkDestroySampler(mContext->mLogicalDevice->mDevice, mSampler, nullptr);
}

void Viewport::Initialize()
{
	mImGuiTextureID.resize(mContext->GetNumFramesInFlight());
	CreateColorBuffer();
	CreateDepthBuffer();
	CreateTextureSampler(mContext->mLogicalDevice->mDevice, mContext->mPhysicalDevice->GetDevice());
}

void Viewport::ImguiSetup()
{
	for (uint32_t i = 0; i < mContext->GetNumFramesInFlight(); i++)
	{
		mImGuiTextureID[i] = (ImTextureID)ImGui_ImplVulkan_AddTexture(
			mSampler,    // You need a Vulkan sampler here
			mColorBuffer[i]->mImageView,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);
	}
}

void Viewport::BeginFrame(VkCommandBuffer iCmd)
{
	if (mWidth != mColorBuffer[mContext->mQueue->GetCurrentInFlightFrame()]->GetExtent().width 
		|| mHeight != mColorBuffer[mContext->mQueue->GetCurrentInFlightFrame()]->GetExtent().height )
	{
		Resize(mWidth, mHeight);
	}
	mColorBuffer[mContext->mQueue->GetCurrentInFlightFrame()]->Transition(iCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void Viewport::EndFrame(VkCommandBuffer iCmd)
{
	mColorBuffer[mContext->mQueue->GetCurrentInFlightFrame()]->Transition(iCmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Viewport::Resize(int iWidth, int iHeight)
{
	ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)mImGuiTextureID[mContext->mQueue->GetCurrentInFlightFrame()]);
	mColorBuffer[mContext->mQueue->GetCurrentInFlightFrame()]->Resize(VkExtent3D{(uint32_t)iWidth, (uint32_t)iHeight, 1 });
	mDepthBuffer[mContext->mQueue->GetCurrentInFlightFrame()]->Resize(VkExtent3D{(uint32_t)iWidth,(uint32_t)iHeight, 1 });
	BindToImgui();
}

VulkanImage* Viewport::GetColorTarget() const
{
	return mColorBuffer[mContext->mQueue->GetCurrentInFlightFrame()].get();
}

VulkanImage* Viewport::GetDepthTarget() const
{
	return mDepthBuffer[mContext->mQueue->GetCurrentInFlightFrame()].get();
}

void Viewport::CreateColorBuffer()
{
	mColorBuffer.resize(mContext->GetNumFramesInFlight());

	VulkanImage::ImageConfig wImageConfig{};
	wImageConfig.format = mColorFormat;
	wImageConfig.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	wImageConfig.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT ;

	for(uint32_t i = 0; i < mContext->GetNumFramesInFlight(); i++)
	{
		mColorBuffer[i] = std::make_unique<VulkanImage>(mContext->mLogicalDevice->mDevice, mContext->mAllocator);
		mColorBuffer[i]->Initialize(VkExtent3D{
			.width = mWidth,
			.height = mHeight,
			.depth = 1
			}, wImageConfig);
	}
}

void Viewport::CreateDepthBuffer()
{
	mDepthBuffer.resize(mContext->GetNumFramesInFlight());

	VulkanImage::ImageConfig wImageConfig{};
	wImageConfig.format = VK_FORMAT_D24_UNORM_S8_UINT;
	wImageConfig.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	wImageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	mContext->mCopyCmd.Reset(0);
	mContext->mCopyCmd.Begin(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (uint32_t i = 0; i < mContext->GetNumFramesInFlight(); i++)
	{
		mDepthBuffer[i] = std::make_unique<VulkanImage>(mContext->mLogicalDevice->mDevice, mContext->mAllocator);
		mDepthBuffer[i]->Initialize(VkExtent3D{
			.width = mWidth,
			.height = mHeight,
			.depth = 1
			}, wImageConfig);


		mDepthBuffer[i]->Transition(mContext->mCopyCmd.mCmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	mContext->mCopyCmd.End();

	mContext->mQueue->SubmitSync(&mContext->mCopyCmd, mContext->mCopyFence.get());
}

ImTextureID Viewport::GetImGuiTextureID() const
{
	return mImGuiTextureID[mContext->mQueue->GetCurrentInFlightFrame()];
}

void Viewport::BindToImgui()
{
	mImGuiTextureID[mContext->mQueue->GetCurrentInFlightFrame()] = (ImTextureID)ImGui_ImplVulkan_AddTexture(
		mSampler,    // You need a Vulkan sampler here
		mColorBuffer[mContext->mQueue->GetCurrentInFlightFrame()]->mImageView,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
}

void Viewport::CreateTextureSampler(VkDevice iDevice, VkPhysicalDevice iPhysicalDevice)
{
	VkSamplerCreateInfo wSamplerInfo{};
	wSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	wSamplerInfo.magFilter = VK_FILTER_LINEAR;
	wSamplerInfo.minFilter = VK_FILTER_LINEAR;
	wSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	wSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	wSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	// Query device properties for anisotropy limits
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(iPhysicalDevice, &properties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(iPhysicalDevice, &deviceFeatures);

	wSamplerInfo.anisotropyEnable = VK_FALSE;
	wSamplerInfo.maxAnisotropy = 1.0f;

	wSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	wSamplerInfo.unnormalizedCoordinates = VK_FALSE;
	wSamplerInfo.compareEnable = VK_FALSE;
	wSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	wSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	wSamplerInfo.mipLodBias = 0.0f;
	wSamplerInfo.minLod = 0.0f;
	wSamplerInfo.maxLod = 1.0;

	VkResult wResult = vkCreateSampler(iDevice, &wSamplerInfo, nullptr, &mSampler);
	CHECK_VK_RESULT(wResult, "Sampler Creation");
}
