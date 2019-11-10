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

void SceneManager::Draw(const Camera& camera)
{
	const glm::mat4& view = camera.GetView();
	const glm::mat4& projection = camera.GetProjection();

	for (auto& entity : m_sceneObjects)
	{
		for (auto& light : m_lightSources) 
		{
			entity->ApplyLightingPass(*light);
		}
		
		entity->Draw(view, projection);
	}
}

void SceneManager::RenderUI()
{

}

