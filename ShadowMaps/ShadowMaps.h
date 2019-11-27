#pragma once

#include "SDLHandler.h"
#include "State.h"

#include "Assets/ShaderManager.h"
#include "SceneManager.h"

#include "Systems/Entity.h"
#include "Systems/Camera/Camera.h"
#include "Systems/Rendering/Skybox.h"
#include "Systems/Rendering/Primitives.h"
#include "Systems/Rendering/Primitives/Quad.h"
#include "Systems/Rendering/Terrain.h"
#include "Systems/Light.h"

#include "Components/System/SceneCameraComponent.h"
#include "RenderPass.h"

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
	SceneManager m_sceneManager;

	bool m_updateOnTick = true;
	std::shared_ptr<SpotLight> m_spotLight;
	std::shared_ptr<DirectionalLight> m_directionalLight;
	std::vector<PointLight> m_pointLights;

private:
	Transform m_lightTransform;
	glm::vec3 m_lightPos;
	Material m_depthMaterial;
	std::shared_ptr<Shader> m_simpleDepthShader;
	std::shared_ptr<Shader> m_shadowMappingShader;
	std::shared_ptr<Shader> m_debugDepthShader;

	Material m_lightMaterial;

	bool m_updateDirLightOnUpdate = false;

	ShadowRenderPass m_shadowRenderPass;
};