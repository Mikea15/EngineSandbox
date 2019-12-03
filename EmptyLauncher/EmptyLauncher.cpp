
#include "EmptyLauncher.h"

#include "Game.h"
#include "Utils.h"
#include "Systems/Rendering/Primitives.h"

#include <iostream>
#include <memory>

#include "EmptyLauncher.h"

int main(int argc, char* argv[])
{
	Game g;
	std::unique_ptr<EmptyLauncher> state = std::make_unique<EmptyLauncher>();
	g.SetState(state.get());
	return g.Execute();
}

void EmptyLauncher::Init(Game* game)
{
	m_sdlHandler = game->GetSDLHandler();
	m_assetManager = game->GetAssetManager();
	m_sceneCamera = &game->GetSystemComponentManager()->GetComponent<SceneCameraComponent>();

	Camera& cam = m_sceneCamera->GetCamera();
	cam.SetPosition(glm::vec3(0.0, 2.0f, 0.0f));

	m_windowParams = game->GetWindowParameters();
	m_tempWindowParams = m_windowParams;

	skyboxShader = m_assetManager->LoadShader("skybox/skybox.vert", "skybox/horizon_sun.frag");
	m_simpleShader = m_assetManager->LoadShader("base.vert", "model_loading.frag");

	m_simpleShader.Use();
	m_simpleShader.SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
	m_simpleShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	// m_model = m_assetManager->LoadModel("nanosuit/nanosuit.obj");
	// m_model = m_assetManager->LoadModel("sponza/sponza_.fbx");
	m_model = m_assetManager->LoadModel("sanmiguel/san-miguel-low-poly.obj");
	m_entity = std::make_shared<Entity>();
	// m_entity->GetTransform().SetPosition(glm::vec3(5.0f, 0.0f, -5.0f));
	// m_entity->GetTransform().SetScale(0.3f);

	m_entity->SetModel(*m_model);

	m_sceneManager.AddEntity(m_entity);

	m_assetManager->RegisterEntity(m_entity);
}

void EmptyLauncher::HandleInput(SDL_Event* event)
{
	if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.sym) 
		{
			default: break;
		}
	}
}

void EmptyLauncher::Update(float deltaTime)
{
	m_sceneManager.Update(deltaTime);
}

void EmptyLauncher::Render(float alpha)
{
	// render
	// ------
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glViewport(0, 0, m_windowParams.Width, m_windowParams.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const Camera& camera = m_sceneCamera->GetCamera();

	glm::mat4 view = camera.GetView();
	glm::mat4 projection = camera.GetProjection();
	glm::vec3 cameraPosition = camera.GetPosition();

	{
		Transform t;
		Material defaultMat = m_assetManager->GetDefaultMaterial();
		defaultMat.SetMVP(t.GetTransform(), view, projection);
		defaultMat.BindTextures();
		defaultMat.GetShader().SetVec3("viewPos", cameraPosition);

		t.SetPosition(glm::vec3(-5.0f, 0.0f, 5.0f));
		defaultMat.GetShader().SetMat4("model", t.GetTransform());
		Primitives::RenderCube();

		t.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
		defaultMat.GetShader().SetMat4("model", t.GetTransform());
		Primitives::RenderCube();

		t.SetPosition(glm::vec3(5.0f, 0.0f, 5.0f));
		defaultMat.GetShader().SetMat4("model", t.GetTransform());
		Primitives::RenderCube();

		// Reset Shader call
		glUseProgram(0);
	}

	m_simpleShader.Use();
	m_simpleShader.SetVec3("viewPos", cameraPosition);

	m_sceneManager.Draw(view, projection);

	// render skybox last. but before transparent objects
	skyboxShader.Use();
	skyboxShader.SetMat4("projection", camera.GetProjection());
	skyboxShader.SetMat4("view", camera.GetView());
	m_skybox.Draw(skyboxShader);
}

void EmptyLauncher::RenderUI()
{
	// m_sceneManager.RenderUI();
	m_skybox.DrawUIPanel();
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
		ImGui::Text("G - Set Spotlight Pos/Direction");
		ImGui::Text("T - Set Directional Light Direction");
	}
	ImGui::End();
}

void EmptyLauncher::Cleanup()
{

}

