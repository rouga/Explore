#include "Model.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

Model::Model(const std::string & iPath)
{
	LoadModel(iPath);
}

void Model::FreeCPU()
{
	for(auto& wMesh : mMeshes)
	{
		wMesh->FreeCPU();
	}
}

void Model::FreeGPU()
{
	for (auto& wMesh : mMeshes)
	{
		wMesh->FreeGPU();
	}
}

void Model::LoadModel(const std::string& iPath)
{
	Assimp::Importer wImporter;
	const aiScene* wScene = wImporter.ReadFile(iPath,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!wScene || wScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !wScene->mRootNode) 
	{
		spdlog::error("Error while loading Model. Assimp Error : {0:s}", wImporter.GetErrorString());
		exit(EXIT_FAILURE);
	}

	mDirectory = iPath.substr(0, iPath.find_last_of('/'));
	ProcessNode(wScene->mRootNode, wScene);
}

void Model::ProcessNode(aiNode* iNode, const aiScene* iScene)
{
	mMeshes.reserve(iScene->mNumMeshes);
	for (unsigned int i = 0; i < iNode->mNumMeshes; i++)
	{
		aiMesh* wMesh = iScene->mMeshes[iNode->mMeshes[i]];
		mMeshes.push_back(std::make_unique<StaticMesh>());
		ProcessMesh(wMesh, iScene, mMeshes.back().get());
		
	}

	for (unsigned int i = 0; i < iNode->mNumChildren; i++) 
	{
		ProcessNode(iNode->mChildren[i], iScene);
	}
}

void Model::ProcessMesh(aiMesh* iAiMesh, const aiScene* iScene, StaticMesh* iStaticMesh)
{
	std::vector<glm::vec3> wPositions;
	std::vector<glm::vec3> wNormals;
	std::vector<glm::vec2> wUVs;
	std::vector<uint32_t> wIndices;


	wPositions.resize(iAiMesh->mNumVertices);
	for (unsigned int i = 0; i < iAiMesh->mNumVertices; i++)
	{
		wPositions[i] = glm::vec3(iAiMesh->mVertices[i].x, iAiMesh->mVertices[i].y, iAiMesh->mVertices[i].z);
	}

	wIndices.resize(iAiMesh->mNumFaces * 3);
	for (unsigned int i = 0; i < iAiMesh->mNumFaces; i++) 
	{
		aiFace face = iAiMesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) 
		{
			wIndices[i*3 + j] = face.mIndices[j];
		}
	}

	iStaticMesh->Initialize(iAiMesh->mName.C_Str(), wPositions, wIndices);

	if(iAiMesh->mNormals)
	{
		wNormals.resize(iAiMesh->mNumVertices);
		for (unsigned int i = 0; i < iAiMesh->mNumVertices; i++)
		{
			wNormals[i] = glm::vec3(iAiMesh->mNormals[i].x, iAiMesh->mNormals[i].y, iAiMesh->mNormals[i].z);
		}
		iStaticMesh->LoadNormals(wNormals);
	}

	if (iAiMesh->mTextureCoords[0])
	{
		wUVs.resize(iAiMesh->mNumVertices);
		for (unsigned int i = 0; i < iAiMesh->mNumVertices; i++)
		{
			wUVs[i] = glm::vec2(iAiMesh->mTextureCoords[0][i].x, iAiMesh->mTextureCoords[0][i].y);
		}
		iStaticMesh->LoadUVs(wUVs);
	}
}
