#include "Renderer/MainPass.h"

#include "Graphics/RenderContext.h"
#include "Core/Engine.h"
#include "Core/Window.h"
#include "Renderer.h"


#include "Core/Logger.h"

#include "Graphics/Utils.h"

MainPass::MainPass(RenderContext* iContext)
	:RenderPass(iContext)
{
}

MainPass::~MainPass()
{
	vkDestroySampler(mContext->mLogicalDevice->mDevice, mSampler, nullptr);
}

void MainPass::Setup(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	CreateTextureSampler(mContext->mLogicalDevice->mDevice, mContext->mPhysicalDevice->GetDevice());

	mRenderPass = std::make_unique<VulkanRenderPass>(mContext->mLogicalDevice->mDevice);

	// Setup Descriptor set layouts
	std::vector<DescriptorSetManager::Binding> wBinding =
	{
		{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
		{1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
		{2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
		{3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE, true},
		{4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT},
		{5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, mSampler}
	};

	mContext->mDescriptorSetManager->CreateLayout(wBinding, "ObjectDS");

	// Setup Pipeline layout
	std::vector<VkDescriptorSetLayout> wDSLayouts = { mContext->mDescriptorSetManager->GetLayout("FrameUB"), mContext->mDescriptorSetManager->GetLayout("ObjectDS") };
	std::vector<VkPushConstantRange> wPCRanges;
	mContext->mPipelineLayoutManager->CreateLayout(wDSLayouts, wPCRanges, "main");

	// Create Pipeline
	mVS = std::make_unique<VulkanShader>();
	mFS = std::make_unique<VulkanShader>();

	mVS->Initialize(mContext->mLogicalDevice->mDevice, "shaders/bin/basic.vert.spv");
	mFS->Initialize(mContext->mLogicalDevice->mDevice, "shaders/bin/basic.frag.spv");

	VulkanGraphicsPipeline::PipelineInfo wPipeline{};
	VulkanGraphicsPipeline::DefaultPipelineConfigInfo(wPipeline);

	wPipeline.pipelineLayout = mContext->mPipelineLayoutManager->GetLayout("main");

	wPipeline.renderingInfo.pColorAttachmentFormats = &iFrameResources->mViewport->GetColorTarget()->GetFormat();
	wPipeline.renderingInfo.depthAttachmentFormat = iFrameResources->mViewport->GetDepthTarget()->GetFormat();
	wPipeline.renderingInfo.stencilAttachmentFormat = iFrameResources->mViewport->GetDepthTarget()->GetFormat();

	mPipeline = std::make_unique<VulkanGraphicsPipeline>();
	mPipeline->Initialize(mContext->mLogicalDevice->mDevice, wPipeline, mVS->mShader, mFS->mShader);

	Logger::Get().mLogger->info("Main pass setup completed.");
}

void MainPass::Begin(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	std::vector<VkImageView> wBackbuffer{ iFrameResources->mViewport->GetColorTarget()->mImageView };

	mRenderPass->Begin(iCmd, wBackbuffer, iFrameResources->mViewport->GetDepthTarget()->mImageView,
		VkExtent2D{ iFrameResources->mViewport->GetWidth(), iFrameResources->mViewport->GetHeight()}
	);
}

void MainPass::Draw(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	uint32_t wCurrentInFlightIndex = mContext->mQueue->GetCurrentInFlightFrame();

	// Bind Global Frame Uniform Buffer
	mPipeline->Bind(iCmd, mContext->mWindow, true);

	VkDescriptorBufferInfo wFrameUBInfo =
	{
		.buffer = iFrameResources->mFrameUniformBuffer->mBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};

	VulkanDescriptorSet wFrameUBDS = mContext->mDescriptorSetManager->AllocateDescriptorSet("FrameUB", wCurrentInFlightIndex);
	wFrameUBDS.Update(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &wFrameUBInfo);
	VkDescriptorSet wFrameUBDSHandle = wFrameUBDS.GetHandle();
	VkDescriptorSet wFrameDSList[] = { wFrameUBDSHandle };

	vkCmdBindDescriptorSets(iCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mContext->mPipelineLayoutManager->GetLayout("main"),
		0, _countof(wFrameDSList), wFrameDSList,
		0, nullptr);

	Model* wModel = Engine::Get().GetModel();

	// Draw Meshes
	for (uint32_t i = 0; i < wModel->GetNumMeshes(); i++)
	{
		StaticMesh* wMesh = wModel->GetMesh(i);
		VkDescriptorBufferInfo wVertexBufferInfo =
		{
			.buffer = wMesh->GetVertexBuffer()->mBuffer,
			.offset = 0,
			.range = VK_WHOLE_SIZE
		};

		VkDescriptorBufferInfo wIndexBufferInfo =
		{
			.buffer = wMesh->GeIndexBuffer()->mBuffer,
			.offset = 0,
			.range = VK_WHOLE_SIZE
		};

		VkDescriptorBufferInfo wNormalBufferInfo =
		{
			.buffer = wMesh->GetNormalBuffer()->mBuffer,
			.offset = 0,
			.range = VK_WHOLE_SIZE
		};

		VkDescriptorBufferInfo wObjectUBInfo =
		{
			.buffer = iFrameResources->mObjectsUniformBuffer->mBuffer,
			.offset = wModel->GetUniformBufferOffset() +  wMesh->GetUniformBufferOffset() + sizeof(ObjectUB) * wCurrentInFlightIndex,
			.range = sizeof(ObjectUB)
		};

		VkDescriptorImageInfo wImageInfo =
		{
			.sampler = mSampler,
			.imageView = wMesh->GetAlbedoTexture()->mImageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		};

		VulkanDescriptorSet wDS = mContext->mDescriptorSetManager->AllocateDescriptorSet("ObjectDS", wCurrentInFlightIndex);
		wDS.Update(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wVertexBufferInfo);
		wDS.Update(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wIndexBufferInfo);
		wDS.Update(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wNormalBufferInfo);

		if(wMesh->isAttributeEnabled(StaticMesh::MeshAttributes::UV))
		{
			VkDescriptorBufferInfo wUVInfo =
			{
				.buffer = wMesh->GetUVBuffer()->mBuffer,
				.offset = 0,
				.range = VK_WHOLE_SIZE
			};

			wDS.Update(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &wUVInfo);
		}

		wDS.Update(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &wObjectUBInfo);
		wDS.Update(5, &wImageInfo);

		VkDescriptorSet wDSHandle = wDS.GetHandle();

		VkDescriptorSet wObjectDSList[] = { wDSHandle };

		vkCmdBindDescriptorSets(iCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mContext->mPipelineLayoutManager->GetLayout("main"),
			1, _countof(wObjectDSList), wObjectDSList,
			0, nullptr);

		vkCmdDraw(iCmd, wMesh->GetIndexCount(), 1, 0, 0);
	}
}

void MainPass::End(VkCommandBuffer iCmd, FrameResources* iFrameResources)
{
	mRenderPass->End(iCmd);
}

void MainPass::CreateTextureSampler(VkDevice iDevice, VkPhysicalDevice iPhysicalDevice)
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

	//if (!deviceFeatures.samplerAnisotropy)
	//{
		wSamplerInfo.anisotropyEnable = VK_FALSE;
		wSamplerInfo.maxAnisotropy = 1.0f;
	//}
	//else
	//{
	//	wSamplerInfo.anisotropyEnable = VK_TRUE;
	//	wSamplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	//}

	wSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	wSamplerInfo.unnormalizedCoordinates = VK_FALSE;
	wSamplerInfo.compareEnable = VK_FALSE;
	wSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	wSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	wSamplerInfo.mipLodBias = 0.0f;
	wSamplerInfo.minLod = 0.0f;
	wSamplerInfo.maxLod = VK_LOD_CLAMP_NONE;

	VkResult wResult = vkCreateSampler(iDevice, &wSamplerInfo, nullptr, &mSampler);
	CHECK_VK_RESULT(wResult, "Sampler Creation");
}
