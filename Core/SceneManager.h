#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "Systems/Camera/Camera.h"
#include "Systems/Entity.h"
#include "Systems/Light.h"


class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void AddEntity(std::shared_ptr<Entity> entity);
	void AddLightSource(std::shared_ptr<ILight> light);

	const std::vector<std::shared_ptr<Entity>>& GetSceneObjects() { return m_sceneObjects; }
	const std::vector<std::shared_ptr<ILight>>& GetSceneLights() { return m_lightSources; }

	void Update(float deltaTime);
	void Draw(const Camera& camera);

	void RenderUI();

private:
	std::vector<std::shared_ptr<Entity>> m_sceneObjects;
	std::vector<std::shared_ptr<ILight>> m_lightSources;
};
