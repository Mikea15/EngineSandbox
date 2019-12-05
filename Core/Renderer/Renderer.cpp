#include "Renderer.h"

#include <imgui.h>
#include <glm/glm.hpp>

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::AddEntity(std::shared_ptr<Entity> entity)
{
	m_sceneObjects.push_back(entity);
}

void Renderer::AddLightSource(std::shared_ptr<ILight> light)
{
	m_lightSources.push_back(light);
}

void Renderer::Update(float deltaTime)
{
	for (auto entity : m_sceneObjects)
	{
		entity->Update(deltaTime);
	}
}

void Renderer::Draw(glm::mat4 cameraView, glm::mat4 cameraProjection)
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

void Renderer::Clear()
{
	m_sceneObjects.clear();
}

void Renderer::RenderUI()
{

}

