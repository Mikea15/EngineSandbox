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
}

void DefaultState::Init(Game* game)
{
	m_sdlHandler = game->GetSDLHandler();
	m_assetManager = game->GetAssetManager();
	m_sceneCamera = &game->GetSystemComponentManager()->GetComponent<SceneCameraComponent>();

	m_windowParams = game->GetWindowParameters();

	currentParams = m_windowParams;

	std::vector<std::string> faces
	{
		std::string("Data/Images/skybox/lake/posx.jpg"),
		std::string("Data/Images/skybox/lake/negx.jpg"),
		std::string("Data/Images/skybox/lake/posy.jpg"),
		std::string("Data/Images/skybox/lake/negy.jpg"),
		std::string("Data/Images/skybox/lake/posz.jpg"),
		std::string("Data/Images/skybox/lake/negz.jpg")
	};

	const unsigned int skyboxTex = m_assetManager->LoadCubemap("lake", faces);

	// m_skybox = Skybox();
	// m_skybox.SetTexture(skyboxTex);

	skyboxShader = shaderManager.LoadShader("gradientSkybox", "skybox/skybox.vert", "skybox/horizon_sun.frag");
	m_simpleShader = shaderManager.LoadShader("simple_textured", "model_loading.vert", "model_loading.frag");
	// m_simpleShader = shaderManager.LoadShader("wireframeSimple", "unlit/simple.vert", "unlit/color.frag");

	m_model = m_assetManager->LoadModel("Data/Objects/nanosuit/nanosuit.obj");
	m_model->Initialize();
	m_model->SetShader(m_simpleShader);

	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	entity->SetModel(*m_model);
	entity->GetTransform().SetScale(glm::vec3(1.0f));

	m_sceneManager.AddEntity(entity);
}

void DefaultState::HandleInput(SDL_Event * event)
{

}

void DefaultState::Update(float deltaTime)
{
	m_sceneManager.Update(deltaTime);
}

void DefaultState::Render(float alpha)
{
	// render
	// ------
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, m_windowParams.Width, m_windowParams.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const Camera& camera = m_sceneCamera->GetCamera();

	m_sceneManager.Draw(camera);

	Transform t;
	t.RotateLocal(glm::vec3(1.0f, 0.0f, 0.0f), 90.0f);
	t.Scale(10.0f);
	m_simpleShader.SetMat4("model", t.GetModelMat());
	Primitives::RenderQuad();

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

		ImGui::Combo("Resolutions", &currentParams.ResolutionIndex, displayNamesStr);
		ImGui::Checkbox("Fullscreen", &currentParams.Fullscreen);
		ImGui::Checkbox("VSync", &currentParams.VSync);
		ImGui::DragInt("FPS Limit", &currentParams.FPSLimit, 1.0f, 30, 200);

		ImGui::SliderInt("Multisample Buffers", &currentParams.GL_MultiSampleBuffers, 1, 4);
		ImGui::SliderInt("Multisample Samples", &currentParams.GL_MultiSamplesSamples, 1, 32);

		if (ImGui::Button("Apply Changes", ImVec2(140, 30)))
		{
			const bool resolutionChanged = currentParams.ResolutionIndex != m_windowParams.ResolutionIndex
				|| currentParams.VSync != m_windowParams.VSync
				|| currentParams.Fullscreen != m_windowParams.Fullscreen;

			const bool globalSettingsChanged = currentParams.FPSLimit != m_windowParams.FPSLimit
				|| currentParams.GL_MultiSampleBuffers != m_windowParams.GL_MultiSampleBuffers
				|| currentParams.GL_MultiSamplesSamples != m_windowParams.GL_MultiSamplesSamples;

			if (resolutionChanged) 
			{
				currentParams.Width = displayModes[currentParams.ResolutionIndex].w;
				currentParams.Height = displayModes[currentParams.ResolutionIndex].h;
			}

			if (resolutionChanged || globalSettingsChanged)
			{
				m_windowParams = currentParams;
				m_sdlHandler->SetWindowParameters(m_windowParams);
			}
		}
		
		ImGui::End();
	}

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
