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
#include "../Core/RenderPass.h"

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
	glm::vec3 lightPos;
	Shader simpleDepthShader;
	Material depthMaterial;
	Shader shader;
	Shader debugDepthQuad;

	Material light;

	ShadowRenderPass m_shadowRenderPass;
};