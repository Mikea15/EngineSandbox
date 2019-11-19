#include <iostream>
#include <memory>

#include "ShadowMaps.h"

#include "../Core/Game.h"

#include "../Core/Utils.h"
#include "../Core/Systems/Rendering/Primitives.h"

#include "../Core/Systems/Rendering/TextureBuffer.h"

#include "../Core/RenderPass.h"

#include <glm/glm.hpp>

int main(int argc, char* argv[])
{
	Game g;

	std::unique_ptr<ShadowMapState> state = std::make_unique<ShadowMapState>();
	g.SetState(state.get());
	return g.Execute();
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO = 0;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

unsigned int planeVAO = 0;
unsigned int planeVBO = 0;
void renderPlane()
{
	if (quadVAO == 0)
	{
		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float planeVertices[] = {
			// positions            // normals         // texcoords
			 25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

			 25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			 25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};
		// plane VAO
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void renderScene(const Shader& shader)
{
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	shader.SetMat4("model", model);
	renderPlane();
}

void ShadowMapState::Init(Game* game)
{
	m_game = game;
	m_sdlHandler = game->GetSDLHandler();
	m_assetManager = game->GetAssetManager();
	m_sceneCamera = &game->GetSystemComponentManager()->GetComponent<SceneCameraComponent>();

	Camera& cam = m_sceneCamera->GetCamera();

	cam.Move(glm::vec3(0.0f, 2.0f, -2.0f));
	cam.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	m_windowParams = game->GetWindowParameters();

	{
		// lights
		auto pointLight0 = std::make_shared<PointLight>();
		pointLight0->position = glm::vec4(1, 3, 1, 1);
		pointLight0->ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		pointLight0->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
		pointLight0->specular = glm::vec3(1.0f, 1.0f, 1.0f);
		pointLight0->constant = 1.0f;
		pointLight0->linear = 0.09f;
		pointLight0->quadratic = 0.032f;

		m_directionalLight = std::make_shared<DirectionalLight>();
		m_directionalLight->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
		m_directionalLight->ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		m_directionalLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		m_directionalLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);

		m_spotLight = std::make_shared<SpotLight>();
		m_spotLight->position = glm::vec3(3.0f, 3.0f, 0.0f);
		m_spotLight->diffuse = glm::vec3(3.0f, 3.0f, 0.0f) - glm::vec3(0.0f);
		m_spotLight->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		m_spotLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		m_spotLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
		m_spotLight->constant = 1.0f;
		m_spotLight->linear = 0.06f;
		m_spotLight->quadratic = 0.032f;
		m_spotLight->cutOff = 30.0f;
		m_spotLight->outerCutOff = 45.0f;

		m_sceneManager.AddLightSource(pointLight0);
		m_sceneManager.AddLightSource(m_directionalLight);
		m_sceneManager.AddLightSource(m_spotLight);
	}

	// shader configuration
	// --------------------
	shader = m_assetManager->LoadShader("sm", "screen/shadow_mapping.vert", "screen/shadow_mapping.frag");
	simpleDepthShader = m_assetManager->LoadShader("smd", "screen/shadow_depth.vert", "screen/shadow_depth.frag");
	debugDepthQuad = m_assetManager->LoadShader("dq", "screen/debug_quad.vert", "screen/debug_quad.frag");

	depthMaterial.SetShader(simpleDepthShader);
	
	shader.Use();
	shader.SetInt("diffuseTexture", 0);
	shader.SetInt("shadowMap", 1);

	debugDepthQuad.Use();
	debugDepthQuad.SetInt("depthMap", 0);

	auto lightShader = m_assetManager->LoadShader("light", "color.vert", "unlit/white.frag");
	lightShader.Use();
	light.SetShader(lightShader);

	// lighting info
	// -------------
	lightPos = glm::vec3(-2.0f, 6.0f, -1.0f);

	auto model = m_assetManager->LoadModel("Data/Objects/nanosuit/nanosuit.obj");
	model->Initialize();
	model->SetShader(shader);
	// model->SetMaterialOverride(std::make_shared<Material>(m_assetManager->GetDefaultMaterial()));

	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	entity->SetModel(*model);
	entity->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	entity->GetTransform().SetScale(glm::vec3(0.3f));

	m_sceneManager.AddEntity(entity);

	m_shadowRenderPass.Initialize();
	m_shadowRenderPass.SetWindowParams(m_windowParams);
	m_shadowRenderPass.SetShader(simpleDepthShader);
	m_shadowRenderPass.SetDirLight(m_directionalLight);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
}

void ShadowMapState::HandleInput(SDL_Event* event)
{
	if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.sym) {
		case SDLK_g:
		{
			Camera& cam = m_sceneCamera->GetCamera();

			// Update Spotlight Position and Direction;
			m_spotLight->position = cam.GetPosition();
			m_spotLight->direction = cam.GetForward();
		}
		break;
		case SDLK_t:
		{
		}
		break;
		default: break;

		}
	}
}

void ShadowMapState::Update(float deltaTime)
{
	m_sceneManager.Update(deltaTime);

	float time = m_game->GetTimeMS();
	lightPos = glm::vec3(
		sin(time * 2.0f) * 2.0f, 
		2.0f + sin(time) * 0.5f, 
		cos(time * 2.0f) * 2.0f
	);

	m_directionalLight->direction = glm::normalize(lightPos - glm::vec3(0.0f));
}

void ShadowMapState::Render(float alpha)
{
	Camera& cam = m_sceneCamera->GetCamera();
	glm::mat4 projection = cam.GetProjection();
	glm::mat4 view = cam.GetView();
	glm::vec3 camPos = cam.GetPosition();

	// render
	// ------
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1 Shadow Pass
	m_shadowRenderPass.Render(cam, m_sceneManager.GetSceneLights(), m_sceneManager.GetSceneObjects(), depthMaterial, renderScene);
	
	// 2. render scene as normal using the generated depth/shadow map  
	// --------------------------------------------------------------
	glViewport(0, 0, m_windowParams.Width, m_windowParams.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Use();
	shader.SetMat4("projection", projection);
	shader.SetMat4("view", view);
	
	// set light uniforms
	shader.SetVec3("viewPos", camPos);
	shader.SetVec3("lightPos", lightPos);

	glm::mat4 lightSpaceMatrix = m_directionalLight->GetProjectionView();
	shader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_assetManager->GetDefaultTexture().GetId());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_shadowRenderPass.GetDepthMap());
	
	renderScene(shader);

	m_sceneManager.Draw(cam);

	light.GetShader().Use();
	Transform pointLightTransform;
	pointLightTransform.SetPosition(lightPos);
	pointLightTransform.SetScale(glm::vec3(0.1f));
	light.SetMVP(pointLightTransform.GetModelMat(), view, projection);
	Primitives::RenderCube();

	// render Depth map to quad for visual debugging
	// ---------------------------------------------
	debugDepthQuad.Use();
	debugDepthQuad.SetFloat("near_plane", m_directionalLight->GetNearPlane());
	debugDepthQuad.SetFloat("far_plane", m_directionalLight->GetFarPlane());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadowRenderPass.GetDepthMap());
	// Primitives::RenderQuad();
	// renderQuad();
}

void ShadowMapState::RenderUI()
{
	m_sceneManager.RenderUI();
	m_sdlHandler->RenderUI();

	ImGui::Begin("Show Buffers");

	const float screenRatio = m_windowParams.Height / static_cast<float>(m_windowParams.Width);
	const float width = ImGui::GetColumnWidth();
	const float height = width * screenRatio;

	ImGui::Image((void*)(intptr_t)m_shadowRenderPass.GetDepthMap(), ImVec2(width, height), 
		ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();

	ImGui::End();

	ImGui::Begin("Directional Light Settings");

	DirectionalLight currentDirLightParams = *m_directionalLight;

	ImGui::Color3("Ambient", currentDirLightParams.ambient);
	ImGui::Color3("Diffuse", currentDirLightParams.diffuse);
	ImGui::Color3("Specular", currentDirLightParams.specular);

	*m_directionalLight = currentDirLightParams;

	ImGui::End();

	ImGui::Begin("SpotLight Settings");

	SpotLight currentSpotLightParams = *m_spotLight;
	ImGui::Checkbox("Update OnTick", &m_updateOnTick);

	ImGui::Color3("Ambient", currentSpotLightParams.ambient);
	ImGui::Color3("Diffuse", currentSpotLightParams.diffuse);
	ImGui::Color3("Specular", currentSpotLightParams.specular);
	ImGui::Separator();
	ImGui::SliderFloat("Constant", &currentSpotLightParams.constant, 0.0f, 10.0f);
	ImGui::SliderFloat("Linear", &currentSpotLightParams.linear, 0.0f, 1.0f);
	ImGui::SliderFloat("Quadratic", &currentSpotLightParams.quadratic, 0.0f, 5.0f, "%.5f");
	ImGui::Separator();

	float tempCutOff = currentSpotLightParams.cutOff;
	float tempOuterCutOff = currentSpotLightParams.outerCutOff;

	ImGui::SliderFloat("Inner CutOff", &tempCutOff, 0.0f, tempOuterCutOff);
	ImGui::SliderFloat("Outer CutOff", &tempOuterCutOff, tempCutOff, 90.0f);

	currentSpotLightParams.cutOff = tempCutOff;
	currentSpotLightParams.outerCutOff = tempOuterCutOff;

	*m_spotLight = currentSpotLightParams;

	ImGui::End();

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

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (open && ImGui::MenuItem("Close")) open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void ShadowMapState::Cleanup()
{

}
