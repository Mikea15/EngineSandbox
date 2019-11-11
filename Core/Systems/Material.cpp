#include "Material.h"

Material::Material()
{
}

void Material::SetShader(const Shader& shader)
{
	m_shader = shader;
}

void Material::SetModel(const glm::mat4& model)
{
	m_shader.Use();
	m_shader.SetMat4("model", model);
}

void Material::SetView(const glm::mat4& view)
{
	m_shader.Use();
	m_shader.SetMat4("view", view);
}

void Material::SetProjection(const glm::mat4& projection)
{
	m_shader.Use();
	m_shader.SetMat4("projection", projection);
}

void Material::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4 & projection)
{
	m_shader.Use();
	m_shader.SetMat4("view", view);
	m_shader.SetMat4("projection", projection);
	m_shader.SetMat4("model", model);
}

/*
	Support for only one texture per texture type
*/
void Material::BindTextures()
{
	m_shader.Use();
	const unsigned int textureSize = static_cast<unsigned int>(m_textures.size());
	for (unsigned int i = 0; i < textureSize; i++)
	{
		switch (m_textures[i].GetType())
		{
		case TextureType::DiffuseMap:
			glActiveTexture(GL_TEXTURE0);
			m_shader.SetInt("material.diffuse", 0);
			break;
		case TextureType::NormalMap:
			glActiveTexture(GL_TEXTURE4);
			m_shader.SetInt("normalMap", 4);
			break;
		case TextureType::MetallicMap:
			glActiveTexture(GL_TEXTURE5);
			m_shader.SetInt("metallicMap", 5);
			break;
		case TextureType::RoughnessMap:
			glActiveTexture(GL_TEXTURE6);
			m_shader.SetInt("roughnessMap", 6);
			break;
		case TextureType::AOMap:
			glActiveTexture(GL_TEXTURE7);
			m_shader.SetInt("aoMap", 7);
			break;
		case TextureType::HeightMap:
			glActiveTexture(GL_TEXTURE8);
			m_shader.SetInt("heightMap", 8);
			break;
		case TextureType::SpecularMap:
			glActiveTexture(GL_TEXTURE1);
			m_shader.SetInt("material.specular", 1);
			break;
		default: break;
		}

		glBindTexture(GL_TEXTURE_2D, m_textures[i].GetId());
	}
}

void Material::AddTexture(Texture texture)
{
	m_textures.push_back(texture);
}

void Material::AddTexturePath(TextureType type, const std::string& path)
{
	m_texturePathPerType[type].push_back(path);
}

