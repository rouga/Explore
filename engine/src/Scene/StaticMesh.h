#pragma once

#include <memory>
#include <vector>
#include <glm//glm.hpp>
#include <string>

#include "Graphics/VulkanGPUBuffer.h"
#include "Transformation.h"

class VulkanCommandBuffer;
class VulkanImage;
class RenderContext;

class StaticMesh
{
public:

	struct TextureReference
	{
		std::shared_ptr<VulkanImage> mTexture = nullptr;
		std::string mPath;
	};

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

	void UploadGeometry(VulkanCommandBuffer* iCmd , RenderContext* iRenderContext);

	void FreeGPU();
	void FreeCPU();

	std::shared_ptr<Transformation> GetTransform() { return mTranformation; }

	bool isUploaded() const {return mUploaded; }
	bool isAttributeEnabled(MeshAttributes iAttribute) const { return mAttributeMask & iAttribute; }

	std::string GetName() const { return mName; }
	VulkanGPUBuffer* GetVertexBuffer() const { return mVertexBuffer.get(); }
	VulkanGPUBuffer* GeIndexBuffer() const { return mIndexBuffer.get(); }
	VulkanGPUBuffer* GetNormalBuffer() const { return mNormalBuffer.get(); }
	VulkanGPUBuffer* GetUVBuffer() const { return mUVBuffer.get(); }
	uint32_t GetIndexCount() const { return mIndexCount; }

	VulkanImage* GetAlbedoTexture() const { return mAlbedoTexture.mTexture.get(); }
	void SetAlbedo(std::shared_ptr<VulkanImage>, const std::string& iPath);

	void SetUniformBufferOffset(uint32_t iOffset) { mUniformBufferOffset = iOffset; }
	uint32_t GetUniformBufferOffset() const { return mUniformBufferOffset; }

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
	TextureReference mAlbedoTexture;

	uint32_t mUniformBufferOffset = 0;

	bool mUploaded = false;

	std::shared_ptr<Transformation> mTranformation;
};