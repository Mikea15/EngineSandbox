#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "Systems/Camera/Camera.h"
#include "Systems/Entity.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void AddEntity(std::shared_ptr<Entity> entity);

	const std::vector<std::shared_ptr<Entity>>& GetSceneObjects() { return m_sceneObjects; }

	void Update(float deltaTime);
	void Draw(const Camera& camera);
	void RenderUI();

private:
	std::vector<std::shared_ptr<Entity>> m_sceneObjects;
};
