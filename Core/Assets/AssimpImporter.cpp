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

	std::cout << "[Import] Model: " << path << " loading...";
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace /* | aiProcess_GenNormals */);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[Import] Model: " << path << " failed to load.\n";
		std::cerr << "[Error] Assimp: " << importer.GetErrorString() << "\n";
	}

	std::cout << " [ok]\n";

	const std::string currentDirectory = path.substr(0, path.find_last_of('/') + 1);

	if (scene->HasMaterials()) 
	{
		std::cout << "[Import] Materials \n";
		const unsigned int materialCount = scene->mNumMaterials;
		for (unsigned int i = 0; i < materialCount; ++i)
		{
			std::cout << "[Import] ["<< i <<"/"<< materialCount <<"] Material: " << scene->mMaterials[i]->GetName().C_Str();

			auto material = LoadMaterial(scene->mMaterials[i], currentDirectory);
			model->AddMaterial(material);

			std::cout << " [ok]\n";
		}
	}

	if (scene->HasMeshes())
	{
		std::cout << "[Import] Meshes \n";
		const unsigned int meshCount = scene->mNumMeshes;
		for (unsigned int i = 0; i < meshCount; ++i)
		{
			std::cout << "[Import] [" << i << "/" << meshCount << "] Mesh: " << scene->mMeshes[i]->mName.C_Str();

			auto mesh = LoadMesh(scene->mMeshes[i]);
			model->AddMesh(mesh);

			std::cout << " [ok]\n";
		}
	}

	std::cout << "[Import] Model imported.\n";

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
			vertex.TexCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
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


