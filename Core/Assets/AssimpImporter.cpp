#include "AssimpImporter.h"

#include "../Systems/Model.h"
#include "../Systems/Rendering/Mesh.h"

AssimpImporter::AssimpImporter()
{
	m_supportedTypes = {
		aiTextureType_DIFFUSE,
		aiTextureType_NORMALS,
		aiTextureType_SPECULAR,
		aiTextureType_HEIGHT
	};

	m_typeConversion =
	{
		{ aiTextureType_DIFFUSE, TextureType::DiffuseMap },
		{ aiTextureType_NORMALS, TextureType::NormalMap },
		{ aiTextureType_SPECULAR, TextureType::SpecularMap },
		{ aiTextureType_HEIGHT, TextureType::HeightMap }
	};
}

std::shared_ptr<Model> AssimpImporter::LoadModel(const std::string& path)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();

	Assimp::Importer importer;

	// aiPostProcessSteps flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[AssetManager] Model: " << path << " failed to load.\n";
		std::cerr << "[Error] Assimp: " << importer.GetErrorString() << "\n";
		return model;
	}

	const std::string currentDirectory = path.substr(0, path.find_last_of('/') + 1);

	ProcessModelNode(scene, scene->mRootNode, model, currentDirectory);

	return model;
}

std::shared_ptr<Mesh> AssimpImporter::LoadMesh(const aiMesh* mesh)
{
	std::shared_ptr<Mesh> meshData = std::make_shared<Mesh>();

	meshData->SetName(mesh->mName.C_Str());

	std::vector<VertexInfo> vertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		VertexInfo vertex;
		if (mesh->HasPositions()) 
		{
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		}

		if (mesh->HasNormals()) 
		{
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}
		
		if (mesh->HasTangentsAndBitangents()) 
		{
			vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		}

		if (mesh->HasTextureCoords(0))
		{
			vertex.TexCoords0 = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}

		if (mesh->HasTextureCoords(1))
		{
			vertex.TexCoords1 = glm::vec2(
				mesh->mTextureCoords[1][i].x,
				mesh->mTextureCoords[1][i].y
			);
		}

		vertices.push_back(vertex);
	}
	meshData->SetVertices(vertices);

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	meshData->SetIndices(indices);

	meshData->SetMaterialIndex(mesh->mMaterialIndex);
	
	return meshData;
}

std::shared_ptr<Material> AssimpImporter::LoadMaterial(const aiMaterial* material, const std::string& dir)
{
	std::shared_ptr<Material> newMaterial = std::make_shared<Material>();

	for (aiTextureType type : m_supportedTypes)
	{
		const unsigned int textureCount = material->GetTextureCount(type);
		for (unsigned int i = 0; i < textureCount; ++i)
		{
			aiString path;
			material->GetTexture(type, i, &path);
			newMaterial->AddTexturePath(m_typeConversion[type], dir + path.C_Str());
		}
	}

	return newMaterial;
}

TextureType AssimpImporter::GetTextureTypeFrom(aiTextureType type)
{
	return m_typeConversion[type];
}

void AssimpImporter::ProcessModelNode(const aiScene* scene, aiNode* node, std::shared_ptr<Model> model, const std::string& directory)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const aiMesh* currentMesh = scene->mMeshes[node->mMeshes[i]];
		std::shared_ptr<Mesh> loadedMesh = LoadMesh(currentMesh);

		const aiMaterial* currentMaterial = scene->mMaterials[currentMesh->mMaterialIndex];
		std::shared_ptr<Material> loadedMaterial = LoadMaterial(currentMaterial, directory);

		loadedMesh->SetMaterial(loadedMaterial);
		model->AddMesh(loadedMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessModelNode(scene, node->mChildren[i], model, directory);
	}
}


