#pragma once

#include "../Core/SDLHandler.h"
#include "../Core/State.h"

#include "../Core/Assets/ShaderManager.h"
#include "../Core/SceneManager.h"

#include "../Core/Systems/Entity.h"
#include "../Core/Systems/Camera/Camera.h"
#include "../Core/Systems/Rendering/Skybox.h"
#include "../Core/Systems/Rendering/Primitives.h"
#include "../Core/Systems/Rendering/Primitives/Quad.h"
#include "../Core/Systems/Rendering/Terrain.h"
#include "../Core/Systems/Light.h"

#include "../Core/Components/System/SceneCameraComponent.h"

#define N_LIGHTS 10

class DefaultState
	: public State
{
public:
	DefaultState() = default;
	virtual ~DefaultState() = default;

	void Init(Game* game) override;
	void HandleInput(SDL_Event* event) override;
	void Update(float deltaTime) override;
	void Render(float alpha = 1.0f) override;
	void RenderUI() override;
	void Cleanup() override;

protected:
	SDLHandler* m_sdlHandler;
	WindowParams m_windowParams;
	WindowParams m_tempWindowParams;

	SceneCameraComponent* m_sceneCamera;
	AssetManager* m_assetManager;

	SceneManager m_sceneManager;
	
	Shader skyboxShader;
	Skybox m_skybox;
	Skybox::Settings skyboxSettings;

	std::shared_ptr<Model> m_model;

	Shader m_simpleShader;
	Shader m_groundShader;

	Material m_defaultMat;

	DirectionalLight m_directionalLight;
	
	bool m_updateOnTick = true;
	SpotLight m_spotLight;
	std::vector<PointLight> m_pointLights;
};