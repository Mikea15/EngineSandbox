#include "Material.h"

Material::Material()
{ }

Material::Material(Shader* shader)
{
	SetShader(shader);
}

void Material::SetShader(Shader* shader)
{
	m_shader = shader;
}

void Material::SetModel(const glm::mat4& model)
{
	m_shader->Use();
	m_shader->SetMatrix("model", model);
}

void Material::SetView(const glm::mat4& view)
{
	m_shader->Use();
	m_shader->SetMatrix("view", view);
}

void Material::SetProjection(const glm::mat4& projection)
{
	m_shader->Use();
	m_shader->SetMatrix("projection", projection);
}

void Material::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4 & projection)
{
	m_shader->Use();
	m_shader->SetMatrix("view", view);
	m_shader->SetMatrix("projection", projection);
	m_shader->SetMatrix("model", model);
}

// Support for only one texture per texture type
void Material::BindTextures()
{
	m_shader->Use();

	for (Texture tex : m_textures)
	{
		TextureType type = tex.GetType();
		GLenum texPosition = GetTextureSlot(type);
		const std::string texName = GetTextureTypeName(type);

		const std::string name = "material." + texName;

		glActiveTexture(texPosition);
		m_shader->SetInt(name, static_cast<int>(type));
		glBindTexture(GL_TEXTURE_2D, tex.GetId());
	}
}

void Material::AddTexture(Texture texture)
{
	m_textures.push_back(texture);
}

