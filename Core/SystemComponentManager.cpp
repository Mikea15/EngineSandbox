#include "SystemComponentManager.h"

const std::size_t SystemComponent::Type = std::hash<std::string>()(TO_STRING(SystemComponent));

SystemComponentManager::SystemComponentManager()
{
}

SystemComponentManager::~SystemComponentManager()
{
	m_components.clear();
}

void SystemComponentManager::Initialize(Game* game)
{
	for (auto& sysComp : m_components)
	{
		sysComp->Initialize(game);
	}
}

void SystemComponentManager::HandleInput(SDL_Event * event)
{
	for (auto& sysComp : m_components)
	{
		sysComp->HandleInput(event);
	}
}

void SystemComponentManager::PreUpdate(float frameTime)
{
	for (auto& sysComp : m_components)
	{
		sysComp->PreUpdate(frameTime);
	}
}

void SystemComponentManager::Update(float deltaTime)
{
	for (auto& sysComp : m_components)
	{
		sysComp->Update(deltaTime);
	}
}

void SystemComponentManager::Render(float alpha)
{
	for (auto& sysComp : m_components)
	{
		sysComp->Render(alpha);
	}
}

void SystemComponentManager::RenderUI()
{
	for (auto& sysComp : m_components)
	{
		sysComp->RenderUI();
	}
}

void SystemComponentManager::Cleanup()
{
	for (auto& sysComp : m_components)
	{
		sysComp->Cleanup();
	}
}
