#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>

#include "StaticMesh.h"

class Model
{
public:
	Model(const std::string& iPath);

	StaticMesh* GetMesh(uint32_t iIndex) {return mMeshes[iIndex].get(); }
	uint32_t GetNumMeshes() const {return mMeshes.size(); }

	void FreeCPU();
	void FreeGPU();

private:
	void LoadModel(const std::string& iPath);
	void ProcessNode(aiNode* iNode, const aiScene* iScene);
	void ProcessMesh(aiMesh* iAiMesh, const aiScene* iScene, StaticMesh* iStaticMesh);

	std::vector<std::unique_ptr<StaticMesh>> mMeshes;
	std::string mDirectory;
};
