#pragma once

#include "Transform.h"
#include "Rendering/Shader.h"

struct ILight
{
public:
	virtual ~ILight() = default;

	virtual void SetProperties(Shader& shader) {};
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

	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
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



