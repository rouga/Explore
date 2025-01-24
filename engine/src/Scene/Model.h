#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <iostream>

#include "StaticMesh.h"
#include "Transformation.h"

class Model
{
public:
	Model(const std::string& iPath);

	StaticMesh* GetMesh(uint32_t iIndex) {return mMeshes[iIndex].get(); }
	uint32_t GetNumMeshes() const {return mMeshes.size(); }

	void FreeCPU();
	void FreeGPU();

	std::shared_ptr<Transformation> GetTransform() { return mTranformation; }

	void SetUniformBufferOffset(uint32_t iOffset) { mUniformBufferOffset = iOffset; }
	uint32_t GetUniformBufferOffset() const { return mUniformBufferOffset; }

private:
	void LoadModel(const std::string& iPath);
	void ProcessNode(aiNode* iNode, const aiScene* iScene, std::shared_ptr<Transformation> iTransformation);
	void ProcessMesh(aiMesh* iAiMesh, const aiScene* iScene, StaticMesh* iStaticMesh);

	std::shared_ptr<Transformation> mTranformation;
	std::vector<std::unique_ptr<StaticMesh>> mMeshes;
	std::string mDirectory;

	uint32_t mUniformBufferOffset = 0;
};
