#include "Model.h"

#include "Core/Logger.h"

#include "Renderer/TextureManager.h"

Model::Model(const std::string & iPath)
{	
	mTranformation = std::make_shared<Transformation>();
	mTranformation->SetParent(nullptr);
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
		Logger::Get().mLogger->error("Error while loading Model. Assimp Error : {0:s}", wImporter.GetErrorString());
		exit(EXIT_FAILURE);
	}

	mDirectory = iPath.substr(0, iPath.find_last_of('/'));
	mMeshes.reserve(wScene->mNumMeshes);
	ProcessNode(wScene->mRootNode, wScene, mTranformation);
}

void Model::ProcessNode(aiNode* iNode, const aiScene* iScene, std::shared_ptr<Transformation> iTransformation)
{
	for (unsigned int i = 0; i < iNode->mNumMeshes; i++)
	{
		aiMesh* wMesh = iScene->mMeshes[iNode->mMeshes[i]];
		mMeshes.push_back(std::make_unique<StaticMesh>());
		ProcessMesh(wMesh, iScene, mMeshes.back().get());
		mMeshes.back().get()->GetTransform()->SetParent(iTransformation);
	}

	for (unsigned int i = 0; i < iNode->mNumChildren; i++) 
	{
		// TO FIX: Currently a hack is implemented for the transform, the last mesh added controls the transform of all children
		ProcessNode(iNode->mChildren[i], iScene, iNode == iScene->mRootNode || mMeshes.empty() ? mTranformation : mMeshes.back().get()->GetTransform());
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

	if (iAiMesh->mMaterialIndex >= 0)
	{
		aiString wTexturePath;
		aiMaterial* wMaterial = iScene->mMaterials[iAiMesh->mMaterialIndex];
		if(wMaterial->GetTextureCount(aiTextureType_DIFFUSE))
		{
			wMaterial->GetTexture(aiTextureType_DIFFUSE,0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().albedoMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().albedoMap.mPath = wPath;
		}
		else if(wMaterial->GetTextureCount(aiTextureType_BASE_COLOR))
		{
			wMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->SetAlbedo(TextureManager::Get().AddTexture(wPath), wPath);
		}
		else
		{
			iStaticMesh->SetAlbedo(TextureManager::Get().mGridTexture, "Grid");
		}

		if (wMaterial->GetTextureCount(aiTextureType_NORMALS))
		{
			wMaterial->GetTexture(aiTextureType_NORMALS, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().normalMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().normalMap.mPath = wPath;
		}
		else if (wMaterial->GetTextureCount(aiTextureType_NORMAL_CAMERA))
		{
			wMaterial->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().normalMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().normalMap.mPath = wPath;
		}

		if (wMaterial->GetTextureCount(aiTextureType_EMISSIVE))
		{
			wMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().emissiveMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().emissiveMap.mPath = wPath;
		}
		else if (wMaterial->GetTextureCount(aiTextureType_EMISSION_COLOR))
		{
			wMaterial->GetTexture(aiTextureType_EMISSION_COLOR, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().emissiveMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().emissiveMap.mPath = wPath;
		}

		if (wMaterial->GetTextureCount(aiTextureType_METALNESS))
		{
			wMaterial->GetTexture(aiTextureType_METALNESS, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().metallicMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().metallicMap.mPath = wPath;
		}

		if (wMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
		{
			wMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().roughnessMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().roughnessMap.mPath = wPath;
		}

		if (wMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
		{
			wMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().aoMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().aoMap.mPath = wPath;
		}
		else if (wMaterial->GetTextureCount(aiTextureType_LIGHTMAP))
		{
			wMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &wTexturePath);
			std::string wPath = mDirectory + "/" + wTexturePath.C_Str();
			iStaticMesh->GetMaterial().aoMap.mTexture = TextureManager::Get().AddTexture(wPath);
			iStaticMesh->GetMaterial().aoMap.mPath = wPath;
		}
	}
	else
	{
		iStaticMesh->GetMaterial().albedoMap.mTexture = TextureManager::Get().mGridTexture;
		iStaticMesh->GetMaterial().albedoMap.mPath = "Grid";
	}
}