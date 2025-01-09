#include "StaticMesh.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>


#include "Graphics/VulkanCommandBuffer.h"
#include "Graphics/RenderContext.h"

StaticMesh::StaticMesh()
{
	mTranformation = std::make_shared<Transformation>();
}

StaticMesh::~StaticMesh()
{
	FreeGPU();
}

void StaticMesh::Initialize(const std::string& iName, const std::vector<glm::vec3>& iPositions, const std::vector<uint32_t>& iIndices)
{
	mName = iName;
	mVertexBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0);
	mIndexBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0);

	mPositions = iPositions;
	mIndices = iIndices;
	mIndexCount = mIndices.size();
}

void StaticMesh::LoadNormals(const std::vector<glm::vec3>& iNormals)
{
	if(iNormals.size() > 0)
	{
		mNormalBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0);
		mNormals = iNormals;
		mAttributeMask |= MeshAttributes::NORMAL;
	}
}

void StaticMesh::LoadUVs(const std::vector<glm::vec2>& iUVs)
{
	if(iUVs.size() > 0)
	{
		mUVBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0);
		mUVs = iUVs;
		mAttributeMask |= MeshAttributes::UV;
	}
}

void StaticMesh::Upload(VulkanCommandBuffer* iCmd, RenderContext* iRenderContext)
{
	uint32_t wVerticesSize = mPositions.size() * sizeof(glm::vec3);
	uint32_t wIndicesSize = mIndices.size() * sizeof(uint32_t);
	uint32_t wNormalsSize = mNormals.size() * sizeof(glm::vec3);
	uint32_t wUVsSize = mUVs.size() * sizeof(glm::vec2);
	uint32_t wLastOffset = 0;
	
	mVertexBuffer->Initialize(iRenderContext->mLogicalDevice.get(), wVerticesSize, iRenderContext->mAllocator);
	mIndexBuffer->Initialize(iRenderContext->mLogicalDevice.get(), wIndicesSize, iRenderContext->mAllocator);

	void* wMappedMem = iRenderContext->mStagingBuffer->MapMemory(0,0);
	memcpy((char*)wMappedMem + wLastOffset, mPositions.data(), wVerticesSize);
	wLastOffset += wVerticesSize;
	memcpy((char*)wMappedMem + wLastOffset, mIndices.data(), wIndicesSize);
	wLastOffset += wIndicesSize;

	if(isAttributeEnabled(MeshAttributes::NORMAL))
	{
		mNormalBuffer->Initialize(iRenderContext->mLogicalDevice.get(), wNormalsSize, iRenderContext->mAllocator);
		memcpy((char*)wMappedMem + wLastOffset, mNormals.data(), wNormalsSize);
		wLastOffset += wNormalsSize;
	}

	if (isAttributeEnabled(MeshAttributes::UV))
	{
		mUVBuffer->Initialize(iRenderContext->mLogicalDevice.get(), wUVsSize, iRenderContext->mAllocator);
		memcpy((char*)wMappedMem + wLastOffset, mUVs.data(), wUVsSize);
		wLastOffset += wUVsSize;
	}

	iRenderContext->mStagingBuffer->UnmapMemory();
	wLastOffset = 0;
	mVertexBuffer->Upload(iCmd, iRenderContext->mStagingBuffer.get(), wVerticesSize, wLastOffset, 0);
	wLastOffset += wVerticesSize;
	mIndexBuffer->Upload(iCmd, iRenderContext->mStagingBuffer.get(), wIndicesSize, wLastOffset, 0);
	wLastOffset += wIndicesSize;

	if (isAttributeEnabled(MeshAttributes::NORMAL))
	{
		mNormalBuffer->Upload(iCmd, iRenderContext->mStagingBuffer.get(), wNormalsSize, wLastOffset, 0);
		wLastOffset += wNormalsSize;
	}

	if (isAttributeEnabled(MeshAttributes::UV))
	{
		mNormalBuffer->Upload(iCmd, iRenderContext->mStagingBuffer.get(), wUVsSize, wLastOffset, 0);
		wLastOffset += wUVsSize;
	}

	mUploaded = true;

	spdlog::info("Mesh {:s} uploaded", mName);
}

void StaticMesh::FreeGPU()
{
	if (mVertexBuffer->mBuffer)
	{
		mVertexBuffer->FreeGPU();
	}

	if (mIndexBuffer->mBuffer)
	{
		mIndexBuffer->FreeGPU();
	}

	if (mNormalBuffer && mNormalBuffer->mBuffer)
	{
		mNormalBuffer->FreeGPU();
	}

	if (mUVBuffer && mUVBuffer->mBuffer)
	{
		mUVBuffer->FreeGPU();
	}
}

void StaticMesh::FreeCPU()
{
	mPositions.clear();
	mIndices.clear();
	mNormals.clear();
	mUVs.clear();
}
