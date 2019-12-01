#include "SceneManager.h"

#include <imgui.h>
#include <glm/glm.hpp>

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::AddEntity(std::shared_ptr<Entity> entity)
{
	m_sceneObjects.push_back(entity);
}

void SceneManager::AddLightSource(std::shared_ptr<ILight> light)
{
	m_lightSources.push_back(light);
}

void SceneManager::Update(float deltaTime)
{
	for (auto entity : m_sceneObjects)
	{
		entity->Update(deltaTime);
	}
}

void SceneManager::Draw(glm::mat4 cameraView, glm::mat4 cameraProjection)
{
	for (auto& entity : m_sceneObjects)
	{
		for (auto& light : m_lightSources) 
		{
			entity->ApplyLightingPass(*light);
		}
		
		entity->Draw(cameraView, cameraProjection);
	}
}

void SceneManager::Clear()
{
	m_sceneObjects.clear();
}

void SceneManager::RenderUI()
{

}

