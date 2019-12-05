#pragma once

#include <vector>

#include "Shader.h"
#include "Systems/Rendering/Texture.h"

#include "Assets/BaseResource.h"
#include "Assets/TextureManager.h"

class Material
	: public BaseResource
{
public:
	Material();
	Material(Shader* shader);
	~Material() = default;

	void SetShader(Shader* shader);
	const Shader* GetShader() const { return m_shader; }

	void SetModel(const glm::mat4& model);
	void SetView(const glm::mat4& view);
	void SetProjection(const glm::mat4& projection);
	void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

	void BindTextures();

	void AddTexture(Texture texture);
	const std::vector<Texture> GetTextures() { return m_textures; }

	bool IsValid() const override { return m_id.IsValid() && m_shader && m_shader->HasValidProgram(); }

private:
	Shader* m_shader = nullptr;
	std::vector<Texture> m_textures;
};

