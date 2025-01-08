#pragma once

#include <memory>
#include <vector>
#include <glm//glm.hpp>
#include <string>

#include "Graphics/VulkanGPUBuffer.h"

class VulkanCommandBuffer;
class RenderContext;

class StaticMesh
{
public:

	enum MeshAttributes {
		NORMAL = 1 << 0,  // 0010
		UV = 1 << 1,  // 0100
	};

	StaticMesh();
	~StaticMesh();

	// Populate mesh CPU side
	void Initialize(const std::string& iName, const std::vector<glm::vec3>& iVertices, const std::vector<uint32_t>& iIndices);
	void LoadNormals(const std::vector<glm::vec3>& iNormals);
	void LoadUVs(const std::vector<glm::vec2>& iUVs);

	void Upload(VulkanCommandBuffer* iCmd , RenderContext* iRenderContext);

	void FreeGPU();
	void FreeCPU();

	bool isUploaded() const {return mUploaded; }
	bool isAttributeEnabled(MeshAttributes iAttribute) const { return mAttributeMask & iAttribute; }

	std::string GetName() const { return mName; }
	VulkanGPUBuffer* GetVertexBuffer() const { return mVertexBuffer.get(); }
	VulkanGPUBuffer* GeIndexBuffer() const { return mIndexBuffer.get(); }
	VulkanGPUBuffer* GetNormalBuffer() const { return mNormalBuffer.get(); }
	VulkanGPUBuffer* GetUVBuffer() const { return mUVBuffer.get(); }
	uint32_t GetIndexCount() const { return mIndexCount; }

private:
	std::string mName;

	std::vector<glm::vec3> mPositions;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec2> mUVs;

	std::vector<uint32_t> mIndices;
	uint32_t mIndexCount = 0;

	unsigned int mAttributeMask = 0x00000000;

	std::unique_ptr<VulkanGPUBuffer> mVertexBuffer = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mIndexBuffer = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mNormalBuffer = nullptr;
	std::unique_ptr<VulkanGPUBuffer> mUVBuffer = nullptr;

	bool mUploaded = false;
};