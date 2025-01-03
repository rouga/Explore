#include "StaticMesh.h"

#include "graphics/VulkanCommandBuffer.h"

StaticMesh::StaticMesh()
{
	mVertexBuffer = std::make_unique<VulkanGPUBuffer>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

StaticMesh::~StaticMesh()
{
	FreeGPU();
}

void StaticMesh::Initialize(const std::vector<Vertex> iVertices)
{
	mVertices = iVertices;
}

void StaticMesh::Upload(VulkanCommandBuffer* iCmd , VulkanLogicalDevice* iDevice, VulkanGPUBuffer* iStagingBuffer)
{
	void* wMappedMem = iStagingBuffer->MapMemory(0,0);
	memcpy(wMappedMem, mVertices.data(), mVertices.size() * sizeof(Vertex));
	iStagingBuffer->UnmapMemory();
	mVertexBuffer->Initialize(iDevice, mVertices.size() * sizeof(Vertex));
	mVertexBuffer->Upload(iCmd, iStagingBuffer, mVertices.size() * sizeof(Vertex), 0, 0);

	mVertices.clear();

	mUploaded = true;
}

void StaticMesh::FreeGPU()
{
	if (mVertexBuffer->mBuffer)
	{
		mVertexBuffer->FreeGPU();
	}
}
