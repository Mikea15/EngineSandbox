#pragma once

#include <vector>
#include <variant>

#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

#include "../Assets/TextureManager.h"

class Material
{
public:
	struct IntermediateData
	{
		TextureType type;
		std::vector<std::string> data;
	};

	struct Property
	{
		std::string name;
		std::variant<int, float, std::string, bool> value;
	};

	Material();
	~Material() = default;

	void SetShader(const Shader& shader);
	Shader& GetShader() { return m_shader; }

	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void SetMaterialProperty(const std::string& name, const std::variant<int, float, std::string, bool>& value);

	void BindTextures();

	void AddTexture(TextureInfo texture);
	void AddTexturePath(TextureType type, const std::string& path);

	std::vector<std::string> GetTexturePaths(TextureType type) { return m_texturePathPerType[type]; }
	const std::vector<std::string> GetTexturePathsConst(TextureType type) const 
	{ 
		const auto findIt = m_texturePathPerType.find(type);
		if (findIt != m_texturePathPerType.end())
		{
			return findIt->second;
		}
		return std::vector<std::string>();
	}

private:
	Shader m_shader;
	std::vector<TextureInfo> m_textures;

	std::unordered_map<TextureType, std::vector<std::string>> m_texturePathPerType;

	std::unordered_map<unsigned int, unsigned int> m_materialMapToVectorIndex;
	std::vector<Property> m_materialProperties;
};

