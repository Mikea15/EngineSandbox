
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

class TerrainDemoState
	: public State
{
public:
	TerrainDemoState() = default;
	virtual ~TerrainDemoState() = default;

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

private:
	Shader m_terrainShader;
	Terrain m_terrain = Terrain(1, 50, 50, 15);
	Transform m_terrainPosition;

	Shader m_skyboxShader;
	Skybox m_skybox;
};