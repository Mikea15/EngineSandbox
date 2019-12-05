#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "Light.h"
#include "Systems/Entity.h"
#include "Systems/Camera/Camera.h"


class Renderer
{
public:
	Renderer();
	~Renderer();

	void AddEntity(std::shared_ptr<Entity> entity);
	void AddLightSource(std::shared_ptr<ILight> light);

	const std::vector<std::shared_ptr<Entity>>& GetSceneObjects() { return m_sceneObjects; }
	const std::vector<std::shared_ptr<ILight>>& GetSceneLights() { return m_lightSources; }

	void Update(float deltaTime);
	void Draw(glm::mat4 cameraView, glm::mat4 cameraProjection);

	void Clear();

	void RenderUI();

private:
	std::vector<std::shared_ptr<Entity>> m_sceneObjects;
	std::vector<std::shared_ptr<ILight>> m_lightSources;
};
