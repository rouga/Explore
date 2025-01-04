#include "StaticMesh.h"

#include "graphics/VulkanCommandBuffer.h"

StaticMesh::StaticMesh()
{
	mVertexBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mIndexBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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

void StaticMesh::Upload(VulkanCommandBuffer* iCmd , VulkanLogicalDevice* iDevice, VulkanGPUBuffer* iStagingBuffer)
{
	uint32_t wVerticesSize = mVertices.size() * sizeof(Vertex);
	uint32_t wIndicesSize = mIndices.size() * sizeof(uint32_t);
	
	void* wMappedMem = iStagingBuffer->MapMemory(0,0);
	memcpy(wMappedMem, mVertices.data(), wVerticesSize);
	memcpy((char*)wMappedMem + wVerticesSize, mIndices.data(), wIndicesSize);
	iStagingBuffer->UnmapMemory();
	
	mVertexBuffer->Initialize(iDevice, wVerticesSize);
	mIndexBuffer->Initialize(iDevice, wIndicesSize);

	mVertexBuffer->Upload(iCmd, iStagingBuffer, wVerticesSize, 0, 0);
	mIndexBuffer->Upload(iCmd, iStagingBuffer, wIndicesSize, wVerticesSize, 0);

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
