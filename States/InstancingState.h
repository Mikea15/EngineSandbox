
#pragma once

#include "DefaultState.h"


class InstancingState
	: public DefaultState
{
public:
	InstancingState() = default;
	~InstancingState() override {};

	void Init(Game* game) override;
	void HandleInput(SDL_Event* event) override;
	void Update(float deltaTime) override;
	void Render(float alpha = 1.0f) override;
	void RenderUI() override;
	void Cleanup() override;

private:
	void GenerateAsteroidBelt();

private:
	unsigned int uboMatrices;

	Shader asteroidShader;
	Shader planetShader;

	Entity rockEnt;
	Entity planetEnt;

	float radius = 50.0f;
	float offset = 15.5f;
	unsigned int asteroidCount = 100;
	int newAsteroidCount = 100;
	std::vector<glm::mat4> modelMatrices;
}; 
