#pragma once

#include "Transform.h"
#include "Rendering/Shader.h"

struct ILight
{
public:
	virtual ~ILight() = default;

	virtual void SetProperties(Shader& shader) {};
	virtual glm::mat4 GetProjectionView() { return glm::mat4(); };
};

struct DirectionalLight
	: public ILight
{
	~DirectionalLight() override = default;

	void SetProperties(Shader& shader) override
	{
		shader.SetVec3("dirLight.direction", direction);

		shader.SetVec3("dirLight.ambient", ambient);
		shader.SetVec3("dirLight.diffuse", diffuse);
		shader.SetVec3("dirLight.specular", specular);
	}

	glm::mat4 GetProjectionView() override
	{
		glm::mat4 lightProjection = glm::ortho(
			-m_orthoSize, m_orthoSize, 
			-m_orthoSize, m_orthoSize, 
			m_nearPlane, m_farPlane);

		glm::mat4 lightView = glm::lookAt(
			-direction * 15.0f,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		return lightProjection * lightView;
	}

	float GetNearPlane() const { return m_nearPlane; }
	float GetFarPlane() const { return m_farPlane; }

	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float m_nearPlane = -1.0f;
	float m_farPlane = 25.0f;
	float m_orthoSize = 7.0f;
};

struct PointLight
	: public ILight
{
	~PointLight() override = default;

	void SetProperties(Shader& shader) override
	{
		shader.SetVec3("pointLights[0].position", position);

		shader.SetVec3("pointLights[0].ambient", ambient);
		shader.SetVec3("pointLights[0].diffuse", diffuse);
		shader.SetVec3("pointLights[0].specular", specular);

		shader.SetFloat("pointLights[0].constant", constant);
		shader.SetFloat("pointLights[0].linear", linear);
		shader.SetFloat("pointLights[0].quadratic", quadratic);
	}

	glm::mat4 GetProjectionView() override
	{
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		return lightSpaceMatrix;
	}

	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
	: public ILight
{
	~SpotLight() override = default;
	void SetProperties(Shader& shader) override
	{
		shader.SetVec3("spotLight.position", position);
		shader.SetVec3("spotLight.direction", direction);

		shader.SetVec3("spotLight.ambient", ambient);
		shader.SetVec3("spotLight.diffuse", diffuse);
		shader.SetVec3("spotLight.specular", specular);

		shader.SetFloat("spotLight.constant", constant);
		shader.SetFloat("spotLight.linear", linear);
		shader.SetFloat("spotLight.quadratic", quadratic);

		shader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(cutOff)));
		shader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));
	}

	glm::mat4 GetProjectionView() override
	{
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(position, direction, glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		return lightSpaceMatrix;
	}

	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};



