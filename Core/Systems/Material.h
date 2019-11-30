#pragma once

#include <vector>

#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

#include "Assets/AssetId.h"
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
	const std::vector<Texture> GetTextures() { return m_textures; }

	AssetId GetId() const { return m_id; }
	bool IsValid() const { return m_id.IsValid() && m_shader.IsValid(); }

private:
	AssetId m_id;

	Shader m_shader;
	std::vector<Texture> m_textures;
};

