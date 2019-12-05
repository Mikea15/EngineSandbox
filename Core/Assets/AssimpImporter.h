#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>
#include <vector>
#include <memory>

#include "AssetId.h"

#include "Systems/Model.h"
#include "Renderer/Material.h"
#include "Systems/Rendering/Mesh.h"
#include "Systems/Rendering/Texture.h"

struct TextureInfo
{
	std::string path;
	TextureType type;
};

struct MaterialInfo
{
	Material material;
	std::vector<TextureInfo> textures;
};

struct LoadedModelInfo
{
	Model model;
	std::vector<Mesh> meshes;
	std::vector<MaterialInfo> materials;
};

class AssimpImporter
{
public:
	AssimpImporter();
	
	LoadedModelInfo LoadModel(const std::string& path);

	Mesh LoadMesh(const aiMesh* mesh);
	MaterialInfo LoadMaterial( const aiMaterial* material, const std::string& dir);

	TextureType GetTextureTypeFrom(aiTextureType type);
	std::vector<aiTextureType> GetSupportedTypes() const { return m_supportedTypes; }

private:
	std::unordered_map<aiTextureType, TextureType> m_typeConversion;
	std::vector<aiTextureType> m_supportedTypes;
};