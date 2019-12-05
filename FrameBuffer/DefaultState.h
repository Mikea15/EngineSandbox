#pragma once

#include "SDLHandler.h"
#include "State.h"

#include "Assets/ShaderManager.h"
#include "Renderer/Renderer.h"

#include "Systems/Entity.h"
#include "Systems/Camera/Camera.h"
#include "Systems/Rendering/Skybox.h"
#include "Systems/Rendering/Primitives.h"
#include "Systems/Rendering/Primitives/Quad.h"
#include "Systems/Rendering/Terrain.h"
#include "Renderer/Light.h"

#include "Components/System/SceneCameraComponent.h"

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
	Renderer m_sceneManager;
	
	Shader* skyboxShader;
	Skybox m_skybox;

	std::shared_ptr<Model> m_model;
	Shader* m_simpleShader;
	Shader* m_groundShader;

	bool m_updateOnTick = true;
	std::shared_ptr<SpotLight> m_spotLight;
	std::shared_ptr<DirectionalLight> m_directionalLight;
	std::vector<PointLight> m_pointLights;

	Shader* m_screenShader;
	Material m_defaultMat;

	// materialShininess
	float shininess = 32.0f;

	unsigned int quadVAO, quadVBO;
	unsigned int m_frameBufferId;
	unsigned int m_textureColorId;
	unsigned int m_renderBufferId;
};