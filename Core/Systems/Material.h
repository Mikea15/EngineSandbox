#pragma once

#include <vector>
#include <variant>

#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

#include "Assets/TextureManager.h"

class Material
{
public:
	Material();
	~Material() = default;

	void SetShader(Shader shader);
	Shader GetShader() { return m_shader; }

	void SetModel(const glm::mat4& model);
	void SetView(const glm::mat4& view);
	void SetProjection(const glm::mat4& projection);
	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

	void BindTextures();

	void AddTexture(Texture texture);
	void AddTexturePath(TextureType type, const std::string& path);

	const std::vector<Texture> GetTextures() { return m_textures; }

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
	std::vector<Texture> m_textures;

	std::unordered_map<TextureType, std::vector<std::string>> m_texturePathPerType;

};

