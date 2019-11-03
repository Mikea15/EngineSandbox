#pragma once

#include "Mesh.h"
#include "Shader.h"

#include "../Transform.h"

class Skybox
{
public:
	struct Settings
	{
		glm::vec3 topColor = glm::vec3(0.22, 0.351, 0.491);
		float topExp = 50.0f;
		glm::vec3 horizonColor = glm::vec3(1, 1, 1);

		glm::vec3 bottomColor = glm::vec3(0.475, 0.316, 0.228);
		float bottomExp = 50.0f;

		float skyIntensity = 1.10f;

		glm::vec3 sunColor = glm::vec3(0.895, 0.93, 0.526);
		float sunIntensity = 1.50f;

		float sunAlpha = 550.0f;
		float sunBeta = 1.0f;

		float sunAzimuth = 175.0f;
		float sunAltitude = 25.0f;
	};

	Skybox();
	~Skybox();

	void Initialize();
	void SetTexture(unsigned int texture) { m_texture = texture; }
	void SetSettings(const Settings& settings) { m_currentSettings = settings; }

	void Draw(const Shader& shader);

	void DrawUIPanel();

	unsigned int GetVAO() const { return m_VAO; }
	unsigned int GetTextureID() { return m_texture; }

private:
	Transform transform;
	unsigned int m_VAO, m_VBO;
	unsigned int m_texture;

	Settings m_currentSettings;
};