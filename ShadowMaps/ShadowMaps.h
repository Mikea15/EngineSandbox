#pragma once

#include "SDLHandler.h"
#include "State.h"

#include "Assets/ShaderManager.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderPass.h"

#include "Systems/Entity.h"
#include "Systems/Camera/Camera.h"
#include "Systems/Rendering/Skybox.h"
#include "Systems/Rendering/Primitives.h"
#include "Systems/Rendering/Primitives/Quad.h"
#include "Systems/Rendering/Terrain.h"
#include "Renderer/Light.h"

#include "Components/System/SceneCameraComponent.h"

class ShadowMapState
	: public State
{
public:
	ShadowMapState() = default;
	virtual ~ShadowMapState() = default;

	void Init(Game* game) override;
	void HandleInput(SDL_Event* event) override;
	void Update(float deltaTime) override;
	void Render(float alpha = 1.0f) override;
	void RenderUI() override;
	void Cleanup() override;

protected:
	Game* m_game;
	SDLHandler* m_sdlHandler;
	WindowParams m_windowParams;

	SceneCameraComponent* m_sceneCamera;
	AssetManager* m_assetManager;
	Renderer m_sceneManager;

	bool m_updateOnTick = true;
	std::shared_ptr<SpotLight> m_spotLight;
	std::shared_ptr<DirectionalLight> m_directionalLight;
	std::vector<PointLight> m_pointLights;

private:
	Transform m_lightTransform;
	glm::vec3 m_lightPos;

	Shader* m_shadowMappingShader;
	Shader* m_debugDepthShader;

	std::shared_ptr<Model> m_model;
	std::shared_ptr<Material> m_lightMaterial;

	bool m_updateDirLightOnUpdate = false;

	ShadowRenderPass m_shadowRenderPass;
};