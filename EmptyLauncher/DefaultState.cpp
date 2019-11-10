#include "DefaultState.h"

#include "../Core/Game.h"
#include "../Core/Utils.h"
#include "../Core/Systems/Rendering/Primitives.h"

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), static_cast<int>(values.size()));
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), static_cast<int>(values.size()));
	}

	void Color3(const char* label, glm::vec3& outColor, ImGuiColorEditFlags flags = ImGuiColorEditFlags_RGB)
	{
		float color[3] = { outColor.x, outColor.y, outColor.z };
		ColorEdit3(label, color, flags);
		outColor = glm::vec3(color[0], color[1], color[2]);
	}
}

void DefaultState::Init(Game* game)
{
	m_sdlHandler = game->GetSDLHandler();
	m_assetManager = game->GetAssetManager();
	m_sceneCamera = &game->GetSystemComponentManager()->GetComponent<SceneCameraComponent>();

	Camera& cam = m_sceneCamera->GetCamera();

	cam.Move(glm::vec3(0.0f, 2.0f, -2.0f));
	cam.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	m_windowParams = game->GetWindowParameters();
	m_tempWindowParams = m_windowParams;

	std::vector<std::string> faces
	{
		std::string("Data/Images/skybox/lake/posx.jpg"),
		std::string("Data/Images/skybox/lake/negx.jpg"),
		std::string("Data/Images/skybox/lake/posy.jpg"),
		std::string("Data/Images/skybox/lake/negy.jpg"),
		std::string("Data/Images/skybox/lake/posz.jpg"),
		std::string("Data/Images/skybox/lake/negz.jpg")
	};

	m_skybox.SetTexture(m_assetManager->LoadCubemap("lake", faces));
	m_skybox.Initialize();

	skyboxShader = m_assetManager->LoadShader("gradientSkybox", "skybox/skybox.vert", "skybox/horizon_sun.frag");
	m_simpleShader = m_assetManager->LoadShader("lighting", "lit/basic.vert", "lit/basic.frag");

	m_simpleShader.Use();
	m_simpleShader.SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
	m_simpleShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	m_groundShader = m_assetManager->LoadShader("groundShader", "model_loading.vert", "model_loading.frag");

	m_model = m_assetManager->LoadModel("Data/Objects/nanosuit/nanosuit.obj");
	m_model->Initialize();
	m_model->SetShader(m_simpleShader);
	// m_model->SetMaterialOverride(std::make_shared<Material>(m_defaultMat));

	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	entity->SetModel(*m_model);
	entity->GetTransform().SetScale(glm::vec3(0.1f));

	m_sceneManager.AddEntity(entity);

	// lights
	PointLight l;
	l.position = glm::vec4(1, 3, 1, 1);

	l.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	l.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	l.specular = glm::vec3(1.0f, 1.0f, 1.0f);

	l.constant = 1.0f;
	l.linear = 0.09f;
	l.quadratic = 0.032f;

	m_pointLights.push_back(l);

	m_directionalLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	m_directionalLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	m_directionalLight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	m_directionalLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);

	m_spotLight.ambient  = glm::vec3(0.0f, 0.0f, 0.0f);
	m_spotLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_spotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_spotLight.constant = 1.0f;
	m_spotLight.linear = 0.09f;
	m_spotLight.quadratic  = 0.032f;
	m_spotLight.cutOff = 12.5f;
	m_spotLight.outerCutOff = 15.0f;
}

void DefaultState::HandleInput(SDL_Event* event)
{
	if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.sym) {
		case SDLK_g:
		{
			Camera& cam = m_sceneCamera->GetCamera();

			// Update Spotlight Position and Direction;
			m_spotLight.position = cam.GetPosition();
			m_spotLight.direction = cam.GetForward();
		}
		case SDLK_t:
		{
			Camera& cam = m_sceneCamera->GetCamera();

			// Update Directional Light direction;
			m_directionalLight.direction = cam.GetForward();
		}
			break;
		default: break;
		}
	}
}

void DefaultState::Update(float deltaTime)
{
	m_sceneManager.Update(deltaTime);

	if (m_updateOnTick)
	{
		Camera& cam = m_sceneCamera->GetCamera();

		// Update Spotlight Position and Direction;
		m_spotLight.position = cam.GetPosition();
		m_spotLight.direction = cam.GetForward();
	}
}

void DefaultState::Render(float alpha)
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
		defaultMat.SetMVP(t.GetModelMat(), view, projection);
		defaultMat.BindTextures();

		defaultMat.GetShader().SetVec3("viewPos", cameraPosition);

		t.SetPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
		defaultMat.GetShader().SetMat4("model", t.GetModelMat());
		Primitives::RenderCube();

		t.SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
		defaultMat.GetShader().SetMat4("model", t.GetModelMat());
		Primitives::RenderCube();

		t.SetPosition(glm::vec3(5.0f, 0.0f, 0.0f));
		defaultMat.GetShader().SetMat4("model", t.GetModelMat());
		Primitives::RenderCube();
	}

	m_simpleShader.Use();
	m_simpleShader.SetVec3("viewPos", cameraPosition);
	// Set Directional light info
	m_directionalLight.SetShaderProperties(m_simpleShader);

	// Set Point Light info
	for (auto& l : m_pointLights)
	{
		l.SetShaderProperties(m_simpleShader);
	}
	
	// Set Spotlight Info
	m_spotLight.SetShaderProperties(m_simpleShader);

	m_sceneManager.Draw(camera);


	// render skybox last. but before transparent objects
	skyboxShader.Use();
	skyboxShader.SetMat4("projection", camera.GetProjection());
	skyboxShader.SetMat4("view", camera.GetView());
	m_skybox.Draw(skyboxShader);
}

void DefaultState::RenderUI()
{
	m_sceneManager.RenderUI();

	m_skybox.DrawUIPanel();

	{
		auto displayModes = m_sdlHandler->GetDisplayModes();
		const unsigned int size = static_cast<unsigned int>(displayModes.size());
		std::vector<std::string> displayNamesStr;

		for (unsigned int i = 0; i < size; ++i)
		{
			const std::string name = m_sdlHandler->GetDisplayModeName(i);
			displayNamesStr.push_back(name);
		}

		ImGui::Begin("Window Parameters");

		ImGui::Combo("Resolutions", &m_tempWindowParams.ResolutionIndex, displayNamesStr);
		ImGui::Checkbox("Fullscreen", &m_tempWindowParams.Fullscreen);
		ImGui::Checkbox("VSync", &m_tempWindowParams.VSync);
		ImGui::DragInt("FPS Limit", &m_tempWindowParams.FPSLimit, 1.0f, 30, 200);

		ImGui::SliderInt("Multisample Buffers", &m_tempWindowParams.GL_MultiSampleBuffers, 1, 4);
		ImGui::SliderInt("Multisample Samples", &m_tempWindowParams.GL_MultiSamplesSamples, 1, 32);

		if (ImGui::Button("Apply Changes", ImVec2(140, 30)))
		{
			const bool resolutionChanged = m_tempWindowParams.ResolutionIndex != m_windowParams.ResolutionIndex
				|| m_tempWindowParams.VSync != m_windowParams.VSync
				|| m_tempWindowParams.Fullscreen != m_windowParams.Fullscreen;

			const bool globalSettingsChanged = m_tempWindowParams.FPSLimit != m_windowParams.FPSLimit
				|| m_tempWindowParams.GL_MultiSampleBuffers != m_windowParams.GL_MultiSampleBuffers
				|| m_tempWindowParams.GL_MultiSamplesSamples != m_windowParams.GL_MultiSamplesSamples;

			if (resolutionChanged) 
			{
				m_tempWindowParams.Width = displayModes[m_tempWindowParams.ResolutionIndex].w;
				m_tempWindowParams.Height = displayModes[m_tempWindowParams.ResolutionIndex].h;
			}

			if (resolutionChanged || globalSettingsChanged)
			{
				m_windowParams = m_tempWindowParams;
				m_sdlHandler->SetWindowParameters(m_windowParams);
			}
		}
		
		ImGui::End();
	}

	ImGui::Begin("Directional Light Settings");

	DirectionalLight currentDirLightParams = m_directionalLight;

	ImGui::Color3("Ambient", currentDirLightParams.ambient);
	ImGui::Color3("Diffuse", currentDirLightParams.diffuse);
	ImGui::Color3("Specular", currentDirLightParams.specular);

	m_directionalLight = currentDirLightParams;

	ImGui::End();

	ImGui::Begin("SpotLight Settings");

	SpotLight currentSpotLightParams = m_spotLight;
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

	m_spotLight = currentSpotLightParams;

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

void DefaultState::Cleanup()
{

}

