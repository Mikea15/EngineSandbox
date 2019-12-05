#include "Skybox.h"

#include "../UI/UIHelper.h"

Skybox::Skybox()
{
	
}

Skybox::~Skybox()
{

}

void Skybox::Initialize()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	transform.SetPosition(glm::vec3(0, 0, 0));
	transform.SetScale(10000.0f);
}

void Skybox::Draw(const Shader* shader)
{
	if (m_VAO == 0)
	{
		Initialize();
	}

	shader->SetVector("TopColor",			m_currentSettings.topColor);
	shader->SetFloat("TopExponent",		m_currentSettings.topExp);
	shader->SetVector("HorizonColor",		m_currentSettings.horizonColor);
	shader->SetVector("BottomColor",		m_currentSettings.bottomColor);
	shader->SetFloat("BottomExponent",	m_currentSettings.bottomExp);
	shader->SetFloat("SkyIntensity",		m_currentSettings.skyIntensity);
	shader->SetVector("SunColor",			m_currentSettings.sunColor);
	shader->SetFloat("SunIntensity",		m_currentSettings.sunIntensity);
	shader->SetFloat("SunAlpha",			m_currentSettings.sunAlpha);
	shader->SetFloat("SunBeta",			m_currentSettings.sunBeta);
	shader->SetFloat("SunAzimuth",		m_currentSettings.sunAzimuth);
	shader->SetFloat("SunAltitude",		m_currentSettings.sunAltitude);

	shader->SetMatrix("model", transform.GetTransform());

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(m_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

void Skybox::DrawUIPanel()
{
	ImGui::Begin("Skybox Settings"); 

	ImGui::ColorEdit3("Top Color", m_currentSettings.topColor);
	ImGui::SliderFloat("Top Exponent", &m_currentSettings.topExp, 0.0f, 100.0f);
	ImGui::Separator();
	ImGui::ColorEdit3("Horizon Color", m_currentSettings.horizonColor);
	ImGui::Separator();
	ImGui::ColorEdit3("Bottom Color", m_currentSettings.bottomColor);
	ImGui::SliderFloat("Bottom Exponent", &m_currentSettings.bottomExp, 0.0f, 100.0f);
	ImGui::Separator();
	ImGui::SliderFloat("Sky Intensity", &m_currentSettings.skyIntensity, 0.0f, 2.0f);
	ImGui::ColorEdit3("Sun Color", m_currentSettings.sunColor);
	ImGui::Separator();
	ImGui::SliderFloat("Sun Intensity", &m_currentSettings.sunIntensity, 0.0f, 3.0f);
	ImGui::SliderFloat("Sun Alpha", &m_currentSettings.sunAlpha, 0.0f, 1000.0f);
	ImGui::SliderFloat("Sun Beta", &m_currentSettings.sunBeta, 0.0f, 1.0f);
	ImGui::SliderFloat("Sun Azimuth (deg)", &m_currentSettings.sunAzimuth, 0.0f, 360.0f);
	ImGui::SliderFloat("Sun Altitude (deg)", &m_currentSettings.sunAltitude, 0.0f, 360.0f);

	ImGui::End();
}
