#include "StaticMesh.h"

#include "graphics/VulkanCommandBuffer.h"
#include "graphics/RenderContext.h"

StaticMesh::StaticMesh()
{
	mVertexBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0);
	mIndexBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0);
}

StaticMesh::~StaticMesh()
{
	FreeGPU();
}

void StaticMesh::Initialize(const std::vector<Vertex> iVertices, const std::vector<uint32_t> iIndices)
{
	mVertices = iVertices;
	mIndices = iIndices;
	mIndexCount = mIndices.size();
}

void StaticMesh::Upload(VulkanCommandBuffer* iCmd, RenderContext* iRenderContext)
{
	uint32_t wVerticesSize = mVertices.size() * sizeof(Vertex);
	uint32_t wIndicesSize = mIndices.size() * sizeof(uint32_t);
	
	void* wMappedMem = iRenderContext->mStagingBuffer->MapMemory(0,0);
	memcpy(wMappedMem, mVertices.data(), wVerticesSize);
	memcpy((char*)wMappedMem + wVerticesSize, mIndices.data(), wIndicesSize);
	iRenderContext->mStagingBuffer->UnmapMemory();
	
	mVertexBuffer->Initialize(iRenderContext->mLogicalDevice.get(), wVerticesSize, iRenderContext->mAllocator);
	mIndexBuffer->Initialize(iRenderContext->mLogicalDevice.get(), wIndicesSize, iRenderContext->mAllocator);

	mVertexBuffer->Upload(iCmd, iRenderContext->mStagingBuffer.get(), wVerticesSize, 0, 0);
	mIndexBuffer->Upload(iCmd, iRenderContext->mStagingBuffer.get(), wIndicesSize, wVerticesSize, 0);

	mUploaded = true;
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
}

void StaticMesh::FreeCPU()
{
	mVertices.clear();
	mIndices.clear();
}
