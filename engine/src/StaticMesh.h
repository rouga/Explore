#pragma once

#include <memory>
#include <vector>
#include <glm//glm.hpp>
#include <string>

#include "graphics/VulkanGPUBuffer.h"

class VulkanCommandBuffer;

struct Vertex
{
	Vertex(const glm::vec3& iPos, const glm::vec2 iTexCoord)
		:mPos(iPos),
		mTexCoord(iTexCoord)
	{}

	glm::vec3 mPos{ 0.f, 0.f, 0.f};
	glm::vec2 mTexCoord{0.f, 0.f};
};

class StaticMesh
{
public:
	StaticMesh();
	~StaticMesh();

	// Populate mesh CPU side
	void Initialize(const std::vector<Vertex> iVertices, const std::vector<uint32_t> iIndices);

	void Upload(VulkanCommandBuffer* iCmd , VulkanLogicalDevice* iDevice, VulkanGPUBuffer* iStagingBuffer);

	void FreeGPU();
	void FreeCPU();

	void SetName(std::string iName) { mName = iName; }

	bool IsUploaded() const {return mUploaded; }

	std::string GetName() const { return mName; }
	VulkanGPUBuffer* GetVertexBuffer() const { return mVertexBuffer.get(); }
	VulkanGPUBuffer* GeIndexBuffer() const { return mIndexBuffer.get(); }
	uint32_t GetIndexCount() const { return mIndexCount; }

private:
	std::string mName;
	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;
	uint32_t mIndexCount = 0;
	std::unique_ptr<VulkanGPUBuffer> mVertexBuffer = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mIndexBuffer = nullptr;
	bool mUploaded = false;
};