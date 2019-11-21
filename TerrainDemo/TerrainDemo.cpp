#include <iostream>
#include <memory>

#include "TerrainDemo.h"

#include "Game.h"
#include "Utils.h"

#include "Systems/Rendering/Primitives.h"
#include "Systems/Rendering/TextureBuffer.h"

#include "RenderPass.h"

#include <glm/glm.hpp>

int main(int argc, char* argv[])
{
	Game g;
	std::unique_ptr<TerrainDemoState> state = std::make_unique<TerrainDemoState>();
	g.SetState(state.get());
	return g.Execute();
}

void TerrainDemoState::Init(Game* game)
{
	m_game = game;
	m_sdlHandler = game->GetSDLHandler();
	m_assetManager = game->GetAssetManager();
	m_sceneCamera = &game->GetSystemComponentManager()->GetComponent<SceneCameraComponent>();

	Camera& cam = m_sceneCamera->GetCamera();

	cam.Move(glm::vec3(0.0f, 2.0f, -2.0f));
	cam.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	m_windowParams = game->GetWindowParameters();


	// shader configuration
	// --------------------
	m_terrainShader = m_assetManager->LoadShader("terrain", "model_loading.vert", "model_loading.frag");
	m_skyboxShader = m_assetManager->LoadShader("gradientSkybox", "skybox/skybox.vert", "skybox/horizon_sun.frag");

	// configure global open gl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	m_terrain.GenerateMesh();
	m_terrainPosition.SetPosition(
		glm::vec3(
			-m_terrain.GetPlaneSize().x * 0.5f,
			0.0f,
			-m_terrain.GetPlaneSize().y * 0.5f)
	);
}

void TerrainDemoState::HandleInput(SDL_Event* event)
{

}

void TerrainDemoState::Update(float deltaTime)
{
	m_sceneManager.Update(deltaTime);
}

void TerrainDemoState::Render(float alpha)
{
	Camera& cam = m_sceneCamera->GetCamera();
	glm::mat4 projection = cam.GetProjection();
	glm::mat4 view = cam.GetView();
	glm::vec3 camPos = cam.GetPosition();

	// render
	// ------
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_terrainShader.Use();
	m_terrainShader.SetMat4("projection", projection);
	m_terrainShader.SetMat4("view", view);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_assetManager->GetDefaultTexture().GetId());

	m_terrainShader.SetMat4("model", m_terrainPosition.GetModelMat());
	m_terrain.GetMesh().Draw();

	// render skybox last. but before transparent objects
	m_skyboxShader.Use();
	m_skyboxShader.SetMat4("projection", projection);
	m_skyboxShader.SetMat4("view", view);
	m_skybox.Draw(m_skyboxShader);
}

void TerrainDemoState::RenderUI()
{
	m_sceneManager.RenderUI();
	m_sdlHandler->RenderUI();

	const float DISTANCE = 10.0f;
	static int corner = 1;
	ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
	ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
	if (corner != -1)
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
	static bool open = true;
	if (ImGui::Begin("Example: Simple overlay", &open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0)
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		ImGui::Text("Quick Info");
		ImGui::Separator();
		ImGui::Text("WASD to Move Camera. Q/E Up & Down. F1 - Release Mouse");
		ImGui::Text("C - Toggle Perspective/Orthographic Camera");
		ImGui::Text("F - Focus on Origin");
		ImGui::Text("R - Reset Camera Position");
	}
	ImGui::End();


	ImGui::Begin("Terrain Settings");

	ImGui::BeginGroup();
	ImGui::Text("Terrain Size");
	glm::vec2 planeSize = m_terrain.GetPlaneSize();
	ImGui::SliderFloat("Width", &planeSize.x, 1.0f, 256.0f);
	ImGui::SliderFloat("Length", &planeSize.y, 1.0f, 256.0f);

	float heightSize = m_terrain.GetHeightSize();
	ImGui::SliderFloat("Height", &heightSize, 1.0f, 64.0f);
	if (heightSize != m_terrain.GetHeightSize())
	{
		m_terrain.SetHeightSize(heightSize);
		m_terrain.UpdateHeightMap();
	}

	if (planeSize != m_terrain.GetPlaneSize())
	{
		m_terrain.SetTerrainSize(planeSize);
	}

	if (ImGui::Button("Regenerate Terrain"))
	{
		m_terrain.GenerateMesh();
		m_terrainPosition.SetPosition(glm::vec3(-planeSize.x * 0.5f, 0.0f, -planeSize.y * 0.5f));
	}

	ImGui::EndGroup();

	ImGui::BeginGroup();
	ImGui::Text("Heightmap Settings");

	HeightmapParams currentParams = m_terrain.GetHeightMapParams();
	ImGui::SliderInt("Octaves", &currentParams.octaves, 1, 10);
	ImGui::SliderFloat("Lacunarity", &currentParams.lacunarity, 0.0f, 1.0f);
	ImGui::SliderFloat("Persistence", &currentParams.persistence, 0, 3);
	if (currentParams != m_terrain.GetHeightMapParams())
	{
		m_terrain.SetHeightMapParams(currentParams);
	}

	ImGui::EndGroup();
	ImGui::End();
}

void TerrainDemoState::Cleanup()
{

}
