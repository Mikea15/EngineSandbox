#pragma once

#include "SDLHandler.h"
#include "State.h"

#include "Assets/ShaderManager.h"
#include "Renderer/Renderer.h"

#include "Logger.h"

#include "Systems/Entity.h"
#include "Systems/Camera/Camera.h"
#include "Systems/Rendering/Skybox.h"
#include "Systems/Rendering/Primitives.h"
#include "Systems/Rendering/Primitives/Quad.h"
#include "Systems/Rendering/Terrain.h"
#include "Renderer/Light.h"

#include "Components/System/SceneCameraComponent.h"

class EmptyLauncher
	: public State
{
public:
	EmptyLauncher() = default;
	virtual ~EmptyLauncher() = default;

	void Init(Game* game) override;
	void HandleInput(SDL_Event* event) override;
	void Update(float deltaTime) override;
	void Render(float alpha = 1.0f) override;
	void RenderUI() override;
	void Cleanup() override;

protected:
	SDLHandler* m_sdlHandler = nullptr;
	SceneCameraComponent* m_sceneCamera = nullptr;
	AssetManager* m_assetManager = nullptr;

	WindowParams m_windowParams;
	WindowParams m_tempWindowParams;

	Renderer m_sceneManager;
	
	Shader* skyboxShader;
	Skybox m_skybox;

	std::shared_ptr<Model> m_model;
	std::shared_ptr<Entity> m_entity;
	Shader* m_simpleShader;
};